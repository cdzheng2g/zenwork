#pragma once
#include <QJsonObject>
#include <QJsonArray>
#include <QString>

struct SchemeTool {
    QString id, name, kind;
    QJsonObject parameters;
    QJsonArray rules;
    int version { 1 };
};
