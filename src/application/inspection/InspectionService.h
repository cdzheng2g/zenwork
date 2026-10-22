#pragma once
#include "application/InspectionRunner.h"
#include "contracts/Trigger.h"
#include <QObject>
#include <QFutureWatcher>
#include <QElapsedTimer>
#include <QTimer>
#include <QThreadPool>
#include "contracts/FrameEnvelope.h"
#include "contracts/DeliveryEvent.h"
#include <functional>

class InspectionService final : public QObject {
    Q_OBJECT
public:
    using Executor = std::function<QJsonObject(const InspectionRequest&)>;
    using ResultSink = std::function<QStringList(const QStringList&, const QJsonObject&)>;
    InspectionService(Executor executor, ResultSink sink, QObject* parent = nullptr,
        int timeoutMs = 60000);
    ~InspectionService() override;
    void updateFrame(const cv::Mat& frame);
    void updateFrame(const FrameEnvelope& frame);
    void clearFrame();
    void recordDelivery(const DeliveryEvent& event);
    bool hasFreshFrame() const;
    bool isRunning() const
    {
        return _active;
    }
    void waitForFinished();
    void shutdown();
    bool requiresRecovery() const { return _recoveryRequired; }
    QString recoveryReason() const { return _recoveryReason; }
    void invalidate()
    {
        ++_generation;
    }
    TriggerDecision admission(TriggerSource source, const InspectionGate& gate) const;
    TriggerDecision start(InspectionRequest request, TriggerSource source, const InspectionGate& gate,
        const QString& toolId = { }, quint64 skippedTriggers = 0);
    QJsonObject result() const
    {
        return _result;
    }
    const cv::Mat& resultFrame() const
    {
        return _request.image;
    }
    bool isToolTest() const
    {
        return _toolTest;
    }
    bool discarded() const
    {
        return _discarded;
    }
    QStringList deliveryErrors() const
    {
        return _deliveryErrors;
    }
signals:
    void finished();
    void faulted(const QString& reason);
    void deliveryUpdated(const DeliveryEvent& event);

private:
    void complete();
    void latchFault(const QString& reason);
    Executor _executor;
    ResultSink _sink;
    QFutureWatcher<QJsonObject> _worker;
    QTimer _deadline;
    QThreadPool _pool;
    FrameEnvelope _frame;
    std::shared_ptr<std::atomic_bool> _cancelled;
    quint64 _localFrameId {0};
    InspectionRequest _request;
    QJsonObject _result;
    QStringList _deliveryErrors;
    quint64 _generation { 0 }, _runningGeneration { 0 }, _sequence { 0 }, _skipped { 0 };
    bool _active { false }, _toolTest { false }, _discarded { false };
    bool _timedOut { false }, _recoveryRequired { false };
    bool _shuttingDown { false };
    QString _recoveryReason;
};
