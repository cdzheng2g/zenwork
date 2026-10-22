#include "ResultDeliveryTracker.h"
#include "infrastructure/communication/ModbusTcpOutputService.h"
#include <QCoreApplication>

ResultDeliveryTracker::ResultDeliveryTracker(QObject* parent, int timeout, int capacity)
    : QObject(parent), _timeoutMs(qMax(1, timeout)), _capacity(qMax(1, capacity))
{
    qRegisterMetaType<DeliveryEvent>();
    _sweep.setInterval(qMin(250, _timeoutMs));
    connect(&_sweep, &QTimer::timeout, this, [this] {
        QList<DeliveryEvent> expired;
        for (const auto& record : _records) {
            if (record.event.state != DeliveryState::Submitted) continue;
            if (monotonicNowNs() - record.event.occurredAtNs >= qint64(_timeoutMs) * 1000000) {
                auto event = record.event;
                event.state = DeliveryState::TimedOut;
                event.error = QStringLiteral("结果交付确认超时");
                event.occurredAtNs = monotonicNowNs();
                expired.append(event);
            }
        }
        for (const auto& event : expired) finish(event);
    });
    _sweep.start();
}
ResultDeliveryTracker& ResultDeliveryTracker::instance()
{
    static auto* tracker = new ResultDeliveryTracker(QCoreApplication::instance());
    static const bool connected = [] {
        QObject::connect(&ModTcpOutputService::instance(), &ModTcpOutputService::deliveryFinished,
            tracker, &ResultDeliveryTracker::finish);
        return true;
    }();
    Q_UNUSED(connected)
    return *tracker;
}
bool ResultDeliveryTracker::begin(const QString& channel, const QJsonObject& result, QString* error)
{
    if (error) error->clear();
    DeliveryEvent event {result["inspectionId"].toString(), result["sequenceText"].toString(), channel};
    const auto id = key(event.inspectionId, channel);
    if (event.inspectionId.isEmpty() || _records.contains(id)) {
        if (error) *error = QStringLiteral("结果交付编号为空或重复");
        return false;
    }
    if (_records.size() >= _capacity) {
        auto oldest = _records.end();
        for (auto it = _records.begin(); it != _records.end(); ++it)
            if (it->event.state != DeliveryState::Submitted
                && (oldest == _records.end() || it->order < oldest->order)) oldest = it;
        if (oldest == _records.end()) {
            if (error) *error = QStringLiteral("待确认结果达到上限，未提交输出");
            return false;
        }
        _records.erase(oldest);
    }
    _records.insert(id, {event, ++_order});
    emit changed(event);
    return true;
}
void ResultDeliveryTracker::finish(const DeliveryEvent& event)
{
    auto it = _records.find(key(event.inspectionId, event.channel));
    if (it == _records.end() || it->event.state != DeliveryState::Submitted
        || event.state == DeliveryState::Submitted) return;
    it->event = event;
    emit changed(event);
}
DeliveryEvent ResultDeliveryTracker::latest(const QString& id, const QString& channel) const
{
    return _records.value(key(id, channel)).event;
}
