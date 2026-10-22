#pragma once
#include <QJsonObject>
#include <QMetaType>
#include <QString>
#include "MonotonicClock.h"

enum class DeliveryState { Submitted, Confirmed, Unconfirmed, Failed, TimedOut };
inline QString deliveryStateName(DeliveryState state)
{
    switch (state) {
    case DeliveryState::Submitted: return "submitted";
    case DeliveryState::Confirmed: return "confirmed";
    case DeliveryState::Unconfirmed: return "unconfirmed";
    case DeliveryState::Failed: return "failed";
    case DeliveryState::TimedOut: return "timed_out";
    }
    return "failed";
}
struct DeliveryEvent {
    QString inspectionId, sequenceText, channel;
    DeliveryState state { DeliveryState::Submitted };
    QString error;
    qint64 occurredAtNs { monotonicNowNs() };
    QJsonObject toJson() const
    {
        return {{"type", "delivery_event"}, {"inspectionId", inspectionId},
            {"sequenceText", sequenceText}, {"channel", channel},
            {"state", deliveryStateName(state)}, {"error", error},
            {"occurredAtNs", QString::number(occurredAtNs)}};
    }
};
Q_DECLARE_METATYPE(DeliveryEvent)
