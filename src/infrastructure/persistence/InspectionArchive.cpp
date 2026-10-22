#include "InspectionArchive.h"
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QStandardPaths>
QString InspectionArchive::append(const QJsonObject& result)
{
    return appendLine(QJsonDocument(result).toJson(QJsonDocument::Compact) + '\n');
}
QString InspectionArchive::appendLine(const QByteArray& bytes)
{
    const auto directory = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/results";
    if (!QDir().mkpath(directory))
        return "无法创建检测记录目录";
    const auto path = directory + "/recent.jsonl";
    if (QFileInfo(path).size() > 5 * 1024 * 1024) {
        if (QFile::exists(path + ".1") && !QFile::remove(path + ".1"))
            return "无法轮换检测记录";
        if (!QFile::rename(path, path + ".1"))
            return "无法轮换检测记录";
    }
    QFile file(path);
    if (!file.open(QIODevice::Append))
        return file.errorString();
    if (file.write(bytes) != bytes.size() || !file.flush())
        return file.errorString();
    return { };
}
