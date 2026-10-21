#pragma once
#include <QJsonObject>
#include <QString>

namespace YamlDocument {
bool read(const QString& path, QJsonObject& object, QString* error = nullptr);
bool write(const QString& path, const QJsonObject& object, QString* error = nullptr);
}
