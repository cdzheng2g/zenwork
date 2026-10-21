#include "application/communication/ResultOutputDispatcher.h"
#include "contracts/CommunicationModule.h"
const QList<OutputChannelDescriptor>& ResultOutputDispatcher::channels()
{
    static const auto channels = [] {
        QList<OutputChannelDescriptor> result;
        for (const auto& m : communicationModules())
            result.append({ m.id, m.name });
        return result;
    }();
    return channels;
}
QStringList ResultOutputDispatcher::publish(const QStringList& enabled, const QJsonObject& result)
{
    QStringList errors;
    for (const auto& id : enabled) {
        const auto* m = communicationModule(id);
        QString error;
        if (!m)
            errors.append("未知输出通道：" + id);
        else if (!m->publish(result, &error))
            errors.append(m->name + "：" + (error.isEmpty() ? QStringLiteral("发送失败") : error));
    }
    return errors;
}
