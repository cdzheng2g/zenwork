#include "InspectionService.h"
#include "application/operators/OperatorRegistry.h"
#include <QtConcurrent/QtConcurrentRun>
#include <exception>
InspectionService::InspectionService(Executor executor, ResultSink sink, QObject* parent, int timeoutMs)
    : QObject(parent)
    , _executor(std::move(executor))
    , _sink(std::move(sink))
{
    connect(&_worker, &QFutureWatcher<QJsonObject>::finished, this, &InspectionService::complete);
    _deadline.setSingleShot(true);
    _deadline.setInterval(qMax(1, timeoutMs));
    connect(&_deadline, &QTimer::timeout, this, [this] {
        // A delayed GUI callback must not fault a task that has already returned.
        if (!_active || _worker.isFinished())
            return;
        _timedOut = true;
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
    _worker.waitForFinished();
}
void InspectionService::waitForFinished()
{
    _worker.waitForFinished();
}
void InspectionService::updateFrame(const cv::Mat& frame)
{
    _frame = frame.clone();
    _frameAge.restart();
}
bool InspectionService::hasFreshFrame() const
{
    return !_frame.empty() && _frameAge.isValid() && _frameAge.elapsed() < 1500;
}
TriggerDecision InspectionService::admission(TriggerSource source, const InspectionGate& gate) const
{
    if (_recoveryRequired)
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
    request.image = _frame.clone();
    request.reference.image = request.reference.image.clone();
    _request = request;
    _toolTest = !toolId.isEmpty();
    _runningGeneration = _generation;
    _skipped = skipped;
    _active = true;
    _timedOut = false;
    _discarded = false;
    _deliveryErrors.clear();
    const auto executor = _executor;
    _worker.setFuture(QtConcurrent::run([request, executor] {
        try {
            return executor(request);
        } catch (const std::exception& e) {
            return QJsonObject { { "scheme", request.scheme }, { "status", "ERROR" },
                { "error", QString::fromUtf8(e.what()) } };
        } catch (...) {
            return QJsonObject { { "scheme", request.scheme }, { "status", "ERROR" },
                { "error", "检测执行异常" } };
        }
    }));
    _deadline.start();
    return TriggerDecision::Accepted;
}
void InspectionService::complete()
{
    _deadline.stop();
    _result = _worker.result();
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
