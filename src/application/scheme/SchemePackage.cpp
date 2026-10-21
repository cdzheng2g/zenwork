#include "SchemePackage.h"
#include "SchemeAssetStore.h"
#include "SchemeCodec.h"
#include "infrastructure/persistence/YamlDocument.h"
#include <QJsonArray>
#include <QSet>
#include <opencv2/imgcodecs.hpp>
#include <cmath>
#include <stdexcept>

namespace {
void require(bool condition, const QString& message)
{
    if (!condition)
        throw std::runtime_error(message.toStdString());
}
void validate(const SchemePackage& p)
{
    require(!p.name.trimmed().isEmpty() && p.name.size() <= 64 && !p.name.contains('/')
            && !p.name.contains('\\') && !p.name.contains(QChar::Null),
        "方案名称无效");
    require(p.settings.triggerIntervalMs >= 50 && p.settings.triggerIntervalMs <= 60000,
        "触发周期必须为 50–60000 ms");
    require(!(p.settings.internalTriggerEnabled && p.settings.modbusTriggerEnabled),
        "内部触发与 Modbus 触发不能同时启用");
    const QString communicationError = validateCommunicationSettings(
        p.settings.communication, p.settings.outputChannels, p.settings.modbusTriggerEnabled);
    require(communicationError.isEmpty(), communicationError);
    const auto invalid = SchemeCodec::validate(p.settings, p.tools);
    require(invalid.isEmpty(), invalid);
    if (!p.settings.referenceModel.isEmpty())
        require(!p.reference.image.empty(), "缺少方案基准图");
    if (!p.reference.image.empty()) {
        const QRectF bounds(0, 0, p.reference.image.cols, p.reference.image.rows);
        const auto rect = p.reference.referenceRect;
        require(std::isfinite(rect.x()) && std::isfinite(rect.y()) && std::isfinite(rect.width())
                && std::isfinite(rect.height())
                && (rect.isNull() || (!rect.isEmpty() && bounds.contains(rect))),
            "基准区域超出图像或无效");
    }
    const auto camera = p.settings.camera;
    const QMap<QString, QPair<double, double>> ranges { { "exposureMs", { 0.1, 100 } }, { "gain", { 1, 16 } },
        { "ispDGain", { 1, 4 } }, { "contrast", { 0, 255 } }, { "saturation", { 0, 255 } },
        { "sharpness", { 0, 100 } } };
    if (!camera.isEmpty()) {
        require(camera.value("autoExposure").isBool(), "相机自动曝光设置无效");
        for (auto it = ranges.begin(); it != ranges.end(); ++it) {
            const auto v = camera.value(it.key());
            require(v.isDouble() && v.toDouble() >= it.value().first && v.toDouble() <= it.value().second,
                "相机参数超出范围：" + it.key());
        }
    }
}
}

bool SchemePackage::save(const QString& path, QString* error, bool includeModels) const
{
    try {
        validate(*this);
        auto portable = *this;
        const auto assets = SchemeAssetStore::prepareForSave(portable, includeModels);
        QJsonObject ref;
        if (!reference.image.empty()) {
            std::vector<uchar> png;
            require(cv::imencode(".png", reference.image, png), "无法编码基准图");
            ref["image"] = SchemeAssetStore::encode(
                QByteArray(reinterpret_cast<const char*>(png.data()), int(png.size())));
            const auto r = reference.referenceRect;
            ref["rect"] = QJsonArray { r.x(), r.y(), r.width(), r.height() };
        }
        auto portableSettings = SchemeCodec::encodeSettings(settings);
        portableSettings.remove("referenceModel");
        QJsonObject root { { "format", "HansAiCameraScheme" }, { "formatVersion", 3 }, { "target", "rk3576" },
            { "name", name }, { "tools", SchemeCodec::encodeTools(portable.tools) }, { "reference", ref },
            { "resources", assets }, { "resourceMode", includeModels ? "embedded" : "shared" },
            { "settings", portableSettings } };
        return YamlDocument::write(path, root, error);
    } catch (const std::exception& e) {
        if (error)
            *error = QString::fromUtf8(e.what());
        return false;
    }
}

bool SchemePackage::load(const QString& path, SchemePackage& package, QString* error)
{
    try {
        QJsonObject root;
        if (!YamlDocument::read(path, root, error))
            return false;
        const int version = root.value("formatVersion").toInt();
        require(root.value("format") == "HansAiCameraScheme" && root["formatVersion"].isDouble()
                && root["formatVersion"].toDouble() == version && (version >= 1 && version <= 3),
            "不支持的方案格式或版本");
        const QString mode = version == 1 ? "embedded" : root.value("resourceMode").toString();
        require(mode == "embedded" || mode == "shared", "方案资源模式无效");
        require(root.value("target") == "rk3576", "该方案的模型目标芯片不是 RK3576");
        require(root.value("tools").isArray() && root.value("settings").isObject(), "缺少方案工具或设置");
        SchemePackage p;
        p.name = root.value("name").toString();
        const auto config = root.value("settings").toObject();
        p.settings = SchemeCodec::decodeSettings(config);
        p.tools = SchemeCodec::decodeTools(root["tools"].toArray(), config["ocr"].toObject());
        const auto ref = root.value("reference").toObject();
        if (!ref.isEmpty()) {
            const auto bytes = SchemeAssetStore::decode(ref.value("image").toObject());
            const std::vector<uchar> encoded(bytes.begin(), bytes.end());
            p.reference.image = cv::imdecode(encoded, cv::IMREAD_COLOR);
            require(!p.reference.image.empty(), "无法解码基准图");
            const auto rect = ref.value("rect").toArray();
            require(rect.size() == 4, "基准区域格式无效");
            for (const auto& v : rect)
                require(v.isDouble(), "基准区域必须是数字");
            p.reference.referenceRect
                = QRectF(rect[0].toDouble(), rect[1].toDouble(), rect[2].toDouble(), rect[3].toDouble());
            p.settings.referenceModel = "embedded";
        }
        SchemeAssetStore::loadInto(p, root.value("resources").toObject(), mode, version < 3);
        validate(p);
        package = std::move(p);
        return true;
    } catch (const std::exception& e) {
        if (error)
            *error = QString::fromUtf8(e.what());
        return false;
    }
}

bool SchemePackage::installResources(
    const QString& directory, SchemeRuntimeSettings& installed, QString* error)
{
    if (!SchemeAssetStore::install(*this, directory, error))
        return false;
    installed = settings;
    return true;
}
