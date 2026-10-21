#include "infrastructure/camera/CameraParamsStore.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>

namespace {
constexpr const char* kParamsSubDir = "camera_params";
constexpr const char* kDefaultModelName = "default";
} // namespace

CameraParamsStore& CameraParamsStore::instance()
{
    static CameraParamsStore store;
    return store;
}

QString CameraParamsStore::filePath(const QString& modelName) const
{
    QString base = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QString name = modelName.isEmpty() ? QString(kDefaultModelName) : modelName;
    return QString("%1/%2/%3.json").arg(base, kParamsSubDir, name);
}

bool CameraParamsStore::save(const QString& modelName, const CameraParams& params) const
{
    QString path = filePath(modelName);
    QDir().mkpath(QFileInfo(path).absolutePath());

    QJsonObject root;
    root["autoExposure"] = params.autoExposure;
    root["exposureMs"] = params.exposureMs;
    root["gain"] = params.gain;
    root["ispDGain"] = params.ispDGain;
    root["contrast"] = params.contrast;
    root["saturation"] = params.saturation;
    root["sharpness"] = params.sharpness;

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly))
        return false;
    file.write(QJsonDocument(root).toJson());
    file.close();
    return true;
}

bool CameraParamsStore::load(const QString& modelName, CameraParams& params) const
{
    QFile file(filePath(modelName));
    if (!file.open(QIODevice::ReadOnly))
        return false;

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();
    if (!doc.isObject())
        return false;

    QJsonObject root = doc.object();
    params.autoExposure = root.value("autoExposure").toBool(true);
    params.exposureMs = root.value("exposureMs").toDouble(10.0);
    params.gain = root.value("gain").toDouble(1.0);
    params.ispDGain = root.value("ispDGain").toDouble(1.0);
    params.contrast = root.value("contrast").toInt(128);
    params.saturation = root.value("saturation").toInt(128);
    params.sharpness = root.value("sharpness").toDouble(100.0);
    return true;
}
