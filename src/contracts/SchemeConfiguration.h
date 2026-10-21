#pragma once
#include <QJsonObject>
#include <QStringList>
#include "SchemeTool.h"
struct SchemeRuntimeSettings {
    bool internalTriggerEnabled { true };
    bool modbusTriggerEnabled { false };
    int triggerIntervalMs { 500 };
    bool logicOr { false };
    QString referenceModel;
    QJsonObject camera;
    QJsonObject communication;
    QStringList outputChannels { QStringLiteral("tcp_server") };
};
