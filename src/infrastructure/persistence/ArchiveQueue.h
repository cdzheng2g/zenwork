#pragma once
#include "contracts/DeliveryEvent.h"
#include <QObject>
#include <QByteArray>
#include <QThread>
#include <functional>
#include <memory>

struct ArchiveState;
class ArchiveThread final : public QThread {
    Q_OBJECT
public:
    explicit ArchiveThread(std::shared_ptr<ArchiveState> state);
signals:
    void saved(const QString& id, const QString& sequence, const QString& error, qint64 atNs);
protected:
    void run() override;
private:
    std::shared_ptr<ArchiveState> _state;
};
class ArchiveQueue final : public QObject {
    Q_OBJECT
public:
    using Writer = std::function<QString(const QByteArray&)>;
    explicit ArchiveQueue(Writer writer, QObject* parent = nullptr,
        int maximumItems = 256, qint64 maximumBytes = 8 * 1024 * 1024);
    ~ArchiveQueue() override;
    bool enqueue(const QJsonObject& value, QString* error = nullptr, bool notify = true);
    bool shutdown(int maximumWaitMs = 2000);
    int pending() const;
signals:
    void completed(const DeliveryEvent& event);
private:
    std::shared_ptr<ArchiveState> _state;
    ArchiveThread* _thread {nullptr};
};
