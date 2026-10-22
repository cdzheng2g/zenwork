#pragma once
#include "contracts/DeliveryEvent.h"
#include <QObject>
#include <QMap>
#include <QTimer>

class ResultDeliveryTracker final : public QObject {
    Q_OBJECT
public:
    explicit ResultDeliveryTracker(QObject* parent = nullptr, int timeoutMs = 5000, int capacity = 256);
    static ResultDeliveryTracker& instance();
    bool begin(const QString& channel, const QJsonObject& result, QString* error = nullptr);
    void finish(const DeliveryEvent& event);
    DeliveryEvent latest(const QString& id, const QString& channel) const;
signals:
    void changed(const DeliveryEvent& event);
private:
    struct Record { DeliveryEvent event; quint64 order; };
    static QString key(const QString& id, const QString& channel) { return id + '/' + channel; }
    QMap<QString, Record> _records;
    QTimer _sweep;
    int _timeoutMs, _capacity;
    quint64 _order {0};
};
