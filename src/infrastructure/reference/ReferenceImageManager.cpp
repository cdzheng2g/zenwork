#include "infrastructure/reference/ReferenceImageManager.h"

#include <opencv2/imgcodecs.hpp>

#include <QDir>
#include <QFile>
#include <QSaveFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>

namespace {
constexpr const char* kReferencesSubDir = "references";
constexpr const char* kImageFileName = "image.png";
constexpr const char* kMetaFileName = "meta.json";
} // namespace

ReferenceImageManager& ReferenceImageManager::instance()
{
    static ReferenceImageManager manager;
    return manager;
}

bool ReferenceImageManager::isValidModelName(const QString& modelName)
{
    const QString name = modelName.trimmed();
    return !name.isEmpty() && name != QStringLiteral(".") && name != QStringLiteral("..")
        && !name.contains('/') && !name.contains('\\') && !name.contains(QChar::Null);
}

ReferenceImageManager::ReferenceImageManager()
{
    loadAllFromDisk();
}

QString ReferenceImageManager::modelDir(const QString& modelName) const
{
    QString base = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    return QString("%1/%2/%3").arg(base, kReferencesSubDir, modelName);
}

QString ReferenceImageManager::imagePath(const QString& modelName) const
{
    return modelDir(modelName) + "/" + kImageFileName;
}

QString ReferenceImageManager::metaPath(const QString& modelName) const
{
    return modelDir(modelName) + "/" + kMetaFileName;
}

void ReferenceImageManager::loadAllFromDisk()
{
    QString base
        = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/" + kReferencesSubDir;
    QDir dir(base);
    if (!dir.exists()) {
        return;
    }
    for (const QString& modelName : dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        ReferenceImage refImage;
        refImage.modelName = modelName;
        refImage.imagePath = imagePath(modelName);
        refImage.image = cv::imread(refImage.imagePath.toStdString(), cv::IMREAD_COLOR);
        if (refImage.image.empty()) {
            continue;
        }
        loadMeta(modelName, refImage);
        _references.insert(modelName, refImage);
    }
    if (!_references.isEmpty() && _currentModel.isEmpty()) {
        _currentModel = _references.firstKey();
    }
}

bool ReferenceImageManager::saveMeta(const QString& modelName) const
{
    auto it = _references.find(modelName);
    if (it == _references.end()) {
        return false;
    }
    const ReferenceImage& refImage = it.value();

    QJsonObject root;
    QJsonObject rectObj;
    rectObj["x"] = refImage.referenceRect.x();
    rectObj["y"] = refImage.referenceRect.y();
    rectObj["width"] = refImage.referenceRect.width();
    rectObj["height"] = refImage.referenceRect.height();
    root["referenceRect"] = rectObj;

    QFile file(metaPath(modelName));
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }
    file.write(QJsonDocument(root).toJson());
    file.close();
    return true;
}

bool ReferenceImageManager::loadMeta(const QString& modelName, ReferenceImage& refImage) const
{
    QFile file(metaPath(modelName));
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();
    if (!doc.isObject()) {
        return false;
    }
    QJsonObject root = doc.object();

    QJsonObject rectObj = root.value("referenceRect").toObject();
    refImage.referenceRect = QRectF(rectObj.value("x").toDouble(), rectObj.value("y").toDouble(),
        rectObj.value("width").toDouble(), rectObj.value("height").toDouble());
    return true;
}

bool ReferenceImageManager::importReferenceImage(const QString& modelName, const QString& sourceImagePath)
{
    const QString normalizedName = modelName.trimmed();
    if (!isValidModelName(normalizedName) || sourceImagePath.isEmpty()) {
        return false;
    }

    try {
        const cv::Mat image = cv::imread(sourceImagePath.toStdString(), cv::IMREAD_COLOR);
        if (image.empty())
            return false;
        return storeReferenceImage(normalizedName, image);
    } catch (const cv::Exception&) {
        // OpenCV 对受损图像或无法写入的路径可能抛异常；界面层只收到导入失败，
        // 不能让异常穿过 Qt 事件循环而终止应用。
        return false;
    }
}

bool ReferenceImageManager::importReferenceImage(const QString& modelName, const cv::Mat& liveFrame)
{
    const QString normalizedName = modelName.trimmed();
    if (!isValidModelName(normalizedName) || liveFrame.empty())
        return false;
    try {
        return storeReferenceImage(normalizedName, liveFrame);
    } catch (const cv::Exception&) {
        return false;
    }
}

bool ReferenceImageManager::storeReferenceImage(const QString& normalizedName, const cv::Mat& image)
{
    const QString directory = modelDir(normalizedName);
    if (!QDir().mkpath(directory))
        return false;

    const QString finalImagePath = imagePath(normalizedName);
    const QString temporaryImagePath = directory + QStringLiteral("/image.import.png");
    if (!cv::imwrite(temporaryImagePath.toStdString(), image))
        return false;
    QFile::remove(finalImagePath);
    if (!QFile::rename(temporaryImagePath, finalImagePath)) {
        QFile::remove(temporaryImagePath);
        return false;
    }

    ReferenceImage refImage;
    refImage.modelName = normalizedName;
    refImage.imagePath = finalImagePath;
    refImage.image = image.clone();
    _references.insert(normalizedName, refImage);
    if (!saveMeta(normalizedName))
        return false;
    if (_currentModel.isEmpty())
        _currentModel = normalizedName;
    return true;
}

bool ReferenceImageManager::setCurrentModel(const QString& modelName)
{
    if (modelName.isEmpty()) {
        _currentModel.clear();
        return true;
    }
    if (!_references.contains(modelName)) {
        return false;
    }
    _currentModel = modelName;
    return true;
}

bool ReferenceImageManager::setReferenceRect(const QString& modelName, const QRectF& rect)
{
    auto it = _references.find(modelName);
    if (it == _references.end()) {
        return false;
    }
    it.value().referenceRect = rect;
    return saveMeta(modelName);
}

QStringList ReferenceImageManager::availableModels() const
{
    return _references.keys();
}

bool ReferenceImageManager::contains(const QString& modelName) const
{
    return _references.contains(modelName);
}

const ReferenceImage* ReferenceImageManager::referenceImage(const QString& modelName) const
{
    auto it = _references.find(modelName);
    if (it == _references.end()) {
        return nullptr;
    }
    return &it.value();
}

const ReferenceImage* ReferenceImageManager::currentReferenceImage() const
{
    return referenceImage(_currentModel);
}

bool ReferenceImageManager::savePreparedReference(ReferenceImage& reference, QString* error)
{
    auto fail = [error](const QString& text) {
        if (error)
            *error = text;
        return false;
    };
    if (!isValidModelName(reference.modelName) || reference.image.empty())
        return fail(QStringLiteral("基准图或型号名称无效"));
    try {
        const QString directory = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
            + "/references/" + reference.modelName;
        if (!QDir().mkpath(directory))
            return fail(QStringLiteral("无法创建基准图目录"));
        std::vector<uchar> png;
        if (!cv::imencode(".png", reference.image, png))
            return fail(QStringLiteral("基准图编码失败"));
        const auto r = reference.referenceRect;
        const QByteArray meta = QJsonDocument(QJsonObject { { "referenceRect",
                                                  QJsonObject { { "x", r.x() }, { "y", r.y() },
                                                      { "width", r.width() }, { "height", r.height() } } } })
                                    .toJson();
        QSaveFile imageFile(directory + "/image.png"), metaFile(directory + "/meta.json");
        if (!imageFile.open(QIODevice::WriteOnly) || !metaFile.open(QIODevice::WriteOnly))
            return fail(QStringLiteral("无法写入基准图文件"));
        if (imageFile.write(reinterpret_cast<const char*>(png.data()), qint64(png.size()))
                != qint64(png.size())
            || metaFile.write(meta) != meta.size() || !imageFile.commit() || !metaFile.commit())
            return fail(QStringLiteral("基准图保存失败，请检查磁盘空间"));
        reference.imagePath = directory + "/image.png";
        return true;
    } catch (const cv::Exception& e) {
        return fail(QString::fromUtf8(e.what()));
    }
}

void ReferenceImageManager::adoptPreparedReference(const ReferenceImage& reference)
{
    _references.insert(reference.modelName, reference);
    if (_currentModel.isEmpty())
        _currentModel = reference.modelName;
}
