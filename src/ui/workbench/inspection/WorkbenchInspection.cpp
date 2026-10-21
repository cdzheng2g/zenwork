#include "ui/workbench/WorkbenchWidget.h"
#include "ui/workbench/LiveMonitorWidget.h"
#include "ui/FileOperation.h"
#include "application/InternalTrigger.h"
#include "infrastructure/reference/ReferenceImageManager.h"
#include <QLabel>
InspectionGate WorkbenchWidget::inspectionGate() const
{
    const auto current = _schemeCatalog.currentSchemeName();
    const auto settings = _schemeCatalog.settings(current);
    return { !current.isEmpty(), !_schemeCatalog.tools(current).isEmpty(), !_editingSchemeName.isEmpty(),
        _filePanel || FileOperation::busy(const_cast<WorkbenchWidget*>(this)), _liveMonitorWidget->isPaused(),
        settings.internalTriggerEnabled, settings.modbusTriggerEnabled, _communicationReady };
}
bool WorkbenchWidget::executeInspection(TriggerSource source, const QString& toolId)
{
    InspectionRequest request;
    request.scheme = _schemeCatalog.currentSchemeName();
    request.settings = _schemeCatalog.settings(request.scheme);
    request.tools = _schemeCatalog.tools(request.scheme);
    if (const auto* ref = ReferenceImageManager::instance().referenceImage(request.settings.referenceModel))
        request.reference = *ref;
    const auto decision
        = _inspection->start(request, source, inspectionGate(), toolId, _internalTrigger->skippedTickCount());
    if (decision != TriggerDecision::Accepted) {
        if (!_inspection->isRunning())
            _internalTrigger->finishExecution();
        _statusLabel->setText(decision == TriggerDecision::Busy ? "正在检测，请稍后重试"
                                                                : "未就绪，请检查方案、编辑状态和相机画面");
        return false;
    }
    _statusLabel->setText("正在检测：" + request.scheme);
    refreshModbusState();
    return true;
}
void WorkbenchWidget::completeInspection()
{
    _internalTrigger->finishExecution();
    refreshModbusState();
    if (_inspection->discarded())
        return;
    const auto result = _inspection->result();
    _liveMonitorWidget->showInspection(result, _inspection->resultFrame(), _inspection->isToolTest());
    if (_inspection->isToolTest()) {
        _statusLabel->setText("单算子测试完成（含必要定位），不计入方案统计或输出");
        return;
    }
    QString message = QStringLiteral("%1：%2，%3 ms")
                          .arg(result["scheme"].toString(), result["status"].toString())
                          .arg(result["elapsedMs"].toDouble());
    if (!_inspection->deliveryErrors().isEmpty())
        message += "；" + _inspection->deliveryErrors().join("；");
    _statusLabel->setText(message);
}
