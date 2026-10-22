#include "DetModule.h"
#include "application/operators/OperatorResources.h"
#include "application/vision/ImageInput.h"
#include "application/vision/detection/DetectionEngine.h"
#include <QDateTime>
#include <QFile>
#include <QFileInfo>
#include <algorithm>
#include <stdexcept>

namespace {
const ResourceSpec modelResource {"modelPath", "models/yolo/yolov5s-640-640.rknn", {}};
const ResourceSpec labelsResource {"labelsPath", "models/yolo/coco_80_labels_list.txt", {}};

ImageInputSpec inputSpec()
{
    ImageInputSpec spec;
    spec.fullImageDefault = true;
    spec.alignedKey = "useAligned";
    spec.alignedDefault = false;
    return spec;
}

class LabelCache
{
public:
    const QStringList& load(const QString& path)
    {
        const QFileInfo info(path);
        if (!info.isFile() || info.size() <= 0 || info.size() > 4 * 1024 * 1024)
            throw std::runtime_error(("标签文件不存在、为空或过大：" + path).toStdString());
        const auto modified = info.lastModified().toMSecsSinceEpoch();
        if (_path == info.absoluteFilePath() && _size == info.size() && _modified == modified)
            return _labels;
        QFile file(path);
        if (!file.open(QIODevice::ReadOnly))
            throw std::runtime_error(("无法读取类别标签：" + path).toStdString());
        const auto bytes = file.readAll();
        if (bytes.size() != info.size()) throw std::runtime_error("类别标签读取不完整");
        auto text = QString::fromUtf8(bytes);
        if (text.startsWith(QChar(0xfeff))) text.remove(0, 1);
        auto labels = text.split('\n');
        if (!labels.isEmpty() && labels.last().trimmed().isEmpty()) labels.removeLast();
        if (labels.isEmpty() || labels.size() > 10000) throw std::runtime_error("类别标签行数无效");
        for (auto& label : labels) {
            label = label.trimmed();
            if (label.isEmpty()) throw std::runtime_error("类别标签不能包含空行");
        }
        _path = info.absoluteFilePath();
        _size = info.size();
        _modified = modified;
        _labels = std::move(labels);
        return _labels;
    }
private:
    QString _path;
    qint64 _size {0}, _modified {0};
    QStringList _labels;
};

void execute(OperatorRunContext& context, const QJsonObject& parameters, OperatorOutcome& outcome)
{
    const auto input = prepareOperatorImage(context, parameters, outcome, inputSpec());
    if (!input) return;

    DetectionOptions options;
    options.modelPath = operatorResourcePath(parameters, modelResource);
    options.decoderId = parameters["decoderId"].toString();
    options.decoderParameters = DetectionDecoderRegistry::configuration(options.decoderId, parameters);
    options.confidenceThreshold = parameters["confidenceThreshold"].toDouble();
    options.nmsThreshold = parameters["nmsThreshold"].toDouble();
    options.preprocess.resizeMode = parameters["resizeMode"].toString() == "letterbox"
        ? ImageResizeMode::Letterbox : ImageResizeMode::Stretch;
    options.preprocess.convertToRgb = parameters["convertToRgb"].toBool();

    thread_local DetectionEngine engine;
    thread_local LabelCache labels;
    options.labels = labels.load(operatorResourcePath(parameters, labelsResource));
    const auto objects = engine.detect(input->image, options);
    QStringList names;
    double maximum = 0;
    for (const auto& object : objects) {
        maximum = std::max(maximum, object.confidence);
        names.append(object.label);
    }
    outcome.fields = {{"count", int(objects.size())}, {"confidence", maximum},
        {"labels", names.join(", ")}, {"regions", detectionRegions(objects, *input)}};
    outcome.state = NodeState::Ok;
}

QJsonArray defaultRules(const QJsonObject&)
{
    return {QJsonObject {{"type", "number_range"}, {"field", "count"},
        {"parameters", QJsonObject {{"minimum", 1}, {"maximum", 1}}}}};
}

QString validate(const QJsonObject& parameters)
{
    try {
        const auto id = parameters["decoderId"].toString();
        DetectionDecoderRegistry::create(id, DetectionDecoderRegistry::configuration(id, parameters));
        return {};
    } catch (const std::exception& error) {
        return QString::fromUtf8(error.what());
    }
}
}

OperatorDefinition detectionDefinition()
{
    OperatorDefinition d;
    d.kind = "detection";
    d.currentVersion = 2;
    d.migrate = [](SchemeTool& tool) {
        const auto mode = tool.parameters.value("resizeMode").toString(QStringLiteral("等比例填充"));
        if (mode == "等比例填充" || mode == "letterbox") tool.parameters["resizeMode"] = "letterbox";
        else if (mode == "拉伸" || mode == "stretch") tool.parameters["resizeMode"] = "stretch";
        else return QStringLiteral("旧检测算子缩放方式无效");
        tool.version = 2;
        return QString();
    };
    d.name = "目标检测";
    d.symbol = "检";
    d.imageInput = inputSpec();
    d.parameters = {
        boolParameter("fullImage", "整图检测", true),
        boolParameter("useAligned", "使用定位校正图", false),
        choiceParameter("decoderId", "模型格式", "yolov5", DetectionDecoderRegistry::ids()),
        fileParameter("modelPath", "检测模型", {}, "RKNN 模型 (*.rknn)", false),
        fileParameter("labelsPath", "类别标签", {}, "类别标签 (*.txt)", false),
        numberParameter("confidenceThreshold", "置信度阈值", .5, 0, 1, 2, .01),
        numberParameter("nmsThreshold", "重叠抑制阈值", .45, 0, 1, 2, .01),
        choiceParameter("resizeMode", "图像缩放", "letterbox", {"letterbox", "stretch"},
            {{"letterbox", "等比例填充"}, {"stretch", "拉伸"}}),
        boolParameter("convertToRgb", "输入转换为 RGB", true)
    };
    d.parameters.append(DetectionDecoderRegistry::configurationSchema());
    d.resources = {modelResource, labelsResource};
    d.outputs = {{"count", "目标数量", ParameterKind::Number, "个"},
        {"confidence", "最高置信度", ParameterKind::Number, {}},
        {"labels", "检测类别", ParameterKind::Text, {}}};
    d.execute = &execute;
    d.defaultRules = &defaultRules;
    d.validate = &validate;
    return d;
}
