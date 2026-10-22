#pragma once
#include "ParameterSpec.h"
#include <functional>
class QObject;
struct CommunicationModule {
    QString id, name, description, help;
    ParameterSchema parameters;
    bool listener { false };
    bool triggerInput { false };
    bool asynchronousDelivery { false };
    std::function<bool(const QJsonObject&, QString*)> start;
    std::function<void()> stop;
    std::function<QString()> status;
    std::function<bool(const QJsonObject&, QString*)> publish;
    std::function<void(QObject*, std::function<void()>, std::function<void(QString)>)> observe;
};
const QList<CommunicationModule>& communicationModules();
inline const CommunicationModule* communicationModule(const QString& id)
{
    for (const auto& module : communicationModules())
        if (module.id == id)
            return &module;
    return nullptr;
}
