#include "InspectionService.h"
#include "application/operators/OperatorRegistry.h"
#include <QtConcurrent/QtConcurrentRun>
#include <exception>
#include <QUuid>
InspectionService::InspectionService(Executor executor, ResultSink sink, QObject* parent, int timeoutMs)
    : QObject(parent)
    , _executor(std::move(executor))
    , _sink(std::move(sink))
{
    _pool.setMaxThreadCount(1);
    _pool.setExpiryTimeout(-1);
    connect(&_worker, &QFutureWatcher<QJsonObject>::finished, this, &InspectionService::complete);
    _deadline.setSingleShot(true);
    _deadline.setInterval(qMax(1, timeoutMs));
    connect(&_deadline, &QTimer::timeout, this, [this] {
        // A delayed GUI callback must not fault a task that has already returned.
        if (!_active || _worker.isFinished())
            return;
        _timedOut = true;
        if (_cancelled) _cancelled->store(true);
        latchFault(QStringLiteral("检测超时，已停止接收新任务；等待后台结束后请重启应用恢复"));
    });
}
void InspectionService::latchFault(const QString& reason)
{
    if (_recoveryRequired)
        return;
    _recoveryRequired = true;
    _recoveryReason = reason;
    emit faulted(reason);
}
InspectionService::~InspectionService()
{
    shutdown();
}
void InspectionService::shutdown()
{
    if (!_shuttingDown) {
        _shuttingDown = true;
        invalidate();
    }
    _deadline.stop();
    if (_cancelled) _cancelled->store(true);
    _worker.waitForFinished();
    _pool.waitForDone();
    if (_active) _discarded = true;
    _active = false;
}
void InspectionService::waitForFinished()
{
    _worker.waitForFinished();
}
void InspectionService::updateFrame(const cv::Mat& frame)
{
    updateFrame(FrameEnvelope {frame, ++_localFrameId, monotonicNowNs()});
}
void InspectionService::updateFrame(const FrameEnvelope& frame)
{
    _frame = frame;
    _frame.image = frame.image.clone();
}
void InspectionService::clearFrame()
{
    _frame = {};
}
void InspectionService::recordDelivery(const DeliveryEvent& event)
{
    if (_result["inspectionId"].toString() == event.inspectionId) {
        auto delivery = _result["delivery"].toObject();
        delivery[event.channel] = event.toJson();
        _result["delivery"] = delivery;
    }
    emit deliveryUpdated(event);
}
bool InspectionService::hasFreshFrame() const
{
    return _frame.fresh();
}
TriggerDecision InspectionService::admission(TriggerSource source, const InspectionGate& gate) const
{
    if (_recoveryRequired || _shuttingDown)
        return TriggerDecision::NotReady;
    return triggerDecision(source, gate, _active, hasFreshFrame());
}
TriggerDecision InspectionService::start(InspectionRequest request, TriggerSource source,
    const InspectionGate& gate, const QString& toolId, quint64 skipped)
{
    const auto decision = admission(source, gate);
    if (decision != TriggerDecision::Accepted)
        return decision;
    if (!toolId.isEmpty()) {
        QList<SchemeTool> selected;
        bool found = false;
        for (const auto& tool : request.tools) {
            if (tool.id == toolId) {
                selected.append(tool);
                found = true;
                break;
            }
            const auto* d = OperatorRegistry::find(tool.kind);
            if (d && d->producesAlignment)
                selected.append(tool);
        }
        if (!found)
            return TriggerDecision::NotReady;
        request.tools = selected;
        request.settings.logicOr = false;
    }
    request.image = _frame.image.clone();
    request.reference.image = request.reference.image.clone();
    request.trace = {QUuid::createUuid().toString(QUuid::WithoutBraces), _frame.id,
        _generation, _frame.acquiredAtNs, monotonicNowNs(), 0};
    _cancelled = std::make_shared<std::atomic_bool>(false);
    request.cancelled = _cancelled;
    _request = request;
    _toolTest = !toolId.isEmpty();
    _runningGeneration = _generation;
    _skipped = skipped;
    _active = true;
    _timedOut = false;
    _discarded = false;
    _deliveryErrors.clear();
    const auto executor = _executor;
    _worker.setFuture(QtConcurrent::run(&_pool, [request, executor]() mutable {
        request.trace.workerStartedAtNs = monotonicNowNs();
        QJsonObject result;
        try {
            result = executor(request);
        } catch (const std::exception& e) {
            result = QJsonObject { { "scheme", request.scheme }, { "status", "ERROR" },
                { "error", QString::fromUtf8(e.what()) } };
        } catch (...) {
            result = QJsonObject { { "scheme", request.scheme }, { "status", "ERROR" },
                { "error", "检测执行异常" } };
        }
        result["inspectionId"] = request.trace.inspectionId;
        result["trace"] = request.trace.toJson();
        return result;
    }));
    _deadline.start();
    return TriggerDecision::Accepted;
}
void InspectionService::complete()
{
    // A completion event can still be queued when shutdown has already joined the worker.
    if (!_active) return;
    _deadline.stop();
    _result = _worker.result();
    const auto trace = InspectionTrace::fromJson(_result["trace"].toObject());
    _result["timings"] = QJsonObject {
        {"frameAgeAtTriggerMs", double(trace.acceptedAtNs - trace.acquiredAtNs) / 1e6},
        {"queueMs", double(trace.workerStartedAtNs - trace.acceptedAtNs) / 1e6},
        {"algorithmMs", _result["elapsedMs"].toDouble()},
        {"completionMs", double(monotonicNowNs() - trace.acceptedAtNs) / 1e6}};
    if (_timedOut) {
        _result["status"] = "ERROR";
        _result["error"] = _recoveryReason;
        _result["recoveryRequired"] = true;
    }
    if (_result["recoveryRequired"].toBool())
        latchFault(_result["error"].toString(QStringLiteral("推理故障，请重启应用恢复")));
    _discarded = _runningGeneration != _generation;
    if (!_discarded && !_toolTest) {
        _result["sequence"] = double(++_sequence);
        _result["sequenceText"] = QString::number(_sequence);
        _result["skippedTriggers"] = double(_skipped);
        try {
            if (_sink)
                _deliveryErrors = _sink(_request.settings.outputChannels, _result);
        } catch (const std::exception& e) {
            _deliveryErrors << QString::fromUtf8(e.what());
        } catch (...) {
            _deliveryErrors << QStringLiteral("结果输出异常");
        }
    }
    _active = false;
    emit finished();
}
