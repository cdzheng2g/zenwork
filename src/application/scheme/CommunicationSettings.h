#pragma once
#include "contracts/CommunicationModule.h"
#include <QSet>
inline QJsonObject defaultCommunicationSettings()
{
    QJsonObject result;
    for (const auto& m : communicationModules())
        result[m.id] = parameterDefaults(m.parameters);
    return result;
}
inline QJsonObject normalizedCommunicationSettings(QJsonObject stored)
{
    for (const auto& m : communicationModules())
        stored[m.id] = parameterDefaults(m.parameters, stored[m.id].toObject());
    return stored;
}
inline QStringList activeCommunicationChannels(QStringList outputs, bool modbusTrigger)
{
    if (modbusTrigger && !outputs.contains("modbus_tcp_server"))
        outputs.append("modbus_tcp_server");
    return outputs;
}
inline QString validateCommunicationSettings(
    const QJsonObject& stored, const QStringList& outputs, bool modbusTrigger, bool requireClientHost = false)
{
    const auto config = normalizedCommunicationSettings(stored);
    const auto active = activeCommunicationChannels(outputs, modbusTrigger);
    QSet<QString> channels;
    QSet<int> ports;
    for (const auto& id : active) {
        const auto* m = communicationModule(id);
        if (!m)
            return "未知通信通道：" + id;
        if (channels.contains(id))
            return "通信通道重复：" + id;
        channels.insert(id);
        if (m->listener) {
            const int port = config[id].toObject()["port"].toInt();
            if (ports.contains(port))
                return "通信服务不能使用相同监听端口";
            ports.insert(port);
        }
    }
    for (const auto& m : communicationModules()) {
        if (stored.contains(m.id) && !stored[m.id].isObject())
            return "通信参数格式无效：" + m.name;
        const auto p = config[m.id].toObject();
        const auto invalid = validateParameters(m.parameters, p);
        if (!invalid.isEmpty())
            return m.name + "：" + invalid;
        if (requireClientHost && active.contains(m.id) && p.contains("host")
            && p["host"].toString().trimmed().isEmpty())
            return "请填写服务器地址";
    }
    return { };
}
