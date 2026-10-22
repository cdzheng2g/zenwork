#include "application/communication/ResultOutputDispatcher.h"
#include "contracts/CommunicationModule.h"
#include "ResultDeliveryTracker.h"
#include <QUuid>
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
QStringList ResultOutputDispatcher::publish(const QStringList& enabled, const QJsonObject& value)
{
    auto result = value;
    if (result["inspectionId"].toString().isEmpty())
        result["inspectionId"] = QUuid::createUuid().toString(QUuid::WithoutBraces);
    QStringList errors;
    auto& tracker = ResultDeliveryTracker::instance();
    for (const auto& id : enabled) {
        const auto* m = communicationModule(id);
        QString error;
        if (!m)
            errors.append("未知输出通道：" + id);
        else {
            if (!tracker.begin(id, result, &error)) { errors.append(error); continue; }
            if (!m->publish(result, &error)) {
                if (error.isEmpty()) error = QStringLiteral("发送失败");
                tracker.finish({result["inspectionId"].toString(), result["sequenceText"].toString(),
                    id, DeliveryState::Failed, error});
                errors.append(m->name + "：" + error);
            } else if (!m->asynchronousDelivery) {
                tracker.finish({result["inspectionId"].toString(), result["sequenceText"].toString(),
                    id, DeliveryState::Unconfirmed, {}});
            }
        }
    }
    return errors;
}
