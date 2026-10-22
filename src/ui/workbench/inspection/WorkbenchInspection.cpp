#include "ui/workbench/WorkbenchWidget.h"
#include "ui/workbench/LiveMonitorWidget.h"
#include "ui/FileOperation.h"
#include "application/InternalTrigger.h"
#include "infrastructure/reference/ReferenceImageManager.h"
#include <QLabel>
bool WorkbenchWidget::executeInspection(TriggerSource source, const QString& toolId)
{
    return _coordinator->submit(source, toolId) == TriggerDecision::Accepted;
}
void WorkbenchWidget::completeInspection()
{
    if (_inspection->discarded())
        return;
    const auto result = _inspection->result();
    _liveMonitorWidget->showInspection(result, _inspection->resultFrame(), _inspection->isToolTest());
    if (_inspection->requiresRecovery()) {
        _statusLabel->setText(_inspection->recoveryReason());
        return;
    }
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
