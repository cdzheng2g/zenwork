#pragma once
#include <QJsonObject>
#include <QByteArray>
class InspectionArchive {
public:
    static QString append(const QJsonObject& result);
    static QString appendLine(const QByteArray& line);
};
