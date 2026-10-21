#pragma once
#include "application/InspectionRunner.h"
#include "contracts/Trigger.h"
#include <QObject>
#include <QFutureWatcher>
#include <QElapsedTimer>
#include <functional>

class InspectionService final : public QObject {
    Q_OBJECT
public:
    using Executor = std::function<QJsonObject(const InspectionRequest&)>;
    using ResultSink = std::function<QStringList(const QStringList&, const QJsonObject&)>;
    InspectionService(Executor executor, ResultSink sink, QObject* parent = nullptr);
    ~InspectionService() override;
    void updateFrame(const cv::Mat& frame);
    bool hasFreshFrame() const;
    bool isRunning() const
    {
        return _active;
    }
    void waitForFinished();
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

private:
    void complete();
    Executor _executor;
    ResultSink _sink;
    QFutureWatcher<QJsonObject> _worker;
    QElapsedTimer _frameAge;
    cv::Mat _frame;
    InspectionRequest _request;
    QJsonObject _result;
    QStringList _deliveryErrors;
    quint64 _generation { 0 }, _runningGeneration { 0 }, _sequence { 0 }, _skipped { 0 };
    bool _active { false }, _toolTest { false }, _discarded { false };
};
