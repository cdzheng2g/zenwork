#include "ui/workbench/WorkbenchWidget.h"
#include "ui/workbench/LiveMonitorWidget.h"
#include "ui/pages/CommunicationSettingsPage.h"
#include "application/communication/SchemeCommunicationRuntime.h"
#include <QTimer>

#include <QButtonGroup>
#include <QAbstractButton>
#include <QDateTime>
#include "app/CreateInspectionService.h"
#include "app/CreateInspectionCoordinator.h"
#include <QLabel>
#include <QStackedWidget>
#include <QVBoxLayout>

WorkbenchWidget::WorkbenchWidget(QWidget* parent)
    : QWidget(parent)
{
    _inspection = createInspectionService(this);
    _coordinator = createInspectionCoordinator(*_inspection, this);
    setObjectName("workbenchRoot");
    setStyleSheet(panelStyle());

    auto* rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);
    rootLayout->addWidget(createTopBar());

    _topStack = new QStackedWidget(this);
    _liveMonitorWidget = new LiveMonitorWidget(_topStack);
    _topStack->addWidget(_liveMonitorWidget);
    connect(_liveMonitorWidget, &LiveMonitorWidget::newSchemeRequested, this, &WorkbenchWidget::createScheme);
    connect(_liveMonitorWidget, &LiveMonitorWidget::editSchemeRequested, this,
        &WorkbenchWidget::editCurrentScheme);
    connect(_liveMonitorWidget, &LiveMonitorWidget::schemeSelectionRequested, this,
        &WorkbenchWidget::selectScheme);
    connect(_liveMonitorWidget, &LiveMonitorWidget::deleteSchemeRequested, this,
        &WorkbenchWidget::deleteCurrentScheme);
    connect(_liveMonitorWidget, &LiveMonitorWidget::importSchemeRequested, this,
        [this] { showSchemeFilePanel(false); });
    connect(_liveMonitorWidget, &LiveMonitorWidget::exportSchemeRequested, this,
        [this] { showSchemeFilePanel(true); });
    _topStack->addWidget(new QWidget(_topStack));
    _topStack->addWidget(createWorkflowWorkspace());
    rootLayout->addWidget(_topStack, 1);
    rootLayout->addWidget(createStatusBar());

    _topGroup->button(0)->setChecked(true);
    _topStack->setCurrentIndex(0);
    // 页面切换期间保持采集链路，避免反复初始化媒体资源。
    connect(_inspection, &InspectionService::finished, this, &WorkbenchWidget::completeInspection);
    connect(_inspection, &InspectionService::faulted, this, [this](const QString& reason) {
        _statusLabel->setText(reason);
    });
    connect(_inspection, &InspectionService::deliveryUpdated, this, [this](const DeliveryEvent& event) {
        if (event.state == DeliveryState::Failed || event.state == DeliveryState::TimedOut)
            _statusLabel->setText(QStringLiteral("检测 %1 的 %2 输出失败：%3")
                .arg(event.sequenceText, event.channel, event.error));
    });
    connect(_coordinator, &InspectionCoordinator::submission, this, [this](TriggerDecision decision) {
        _statusLabel->setText(_inspection->requiresRecovery() ? _inspection->recoveryReason()
            : decision == TriggerDecision::Accepted ? QStringLiteral("正在检测：") + _schemeCatalog.currentSchemeName()
            : decision == TriggerDecision::Busy ? QStringLiteral("正在检测，请稍后重试")
            : QStringLiteral("未就绪，请检查方案、编辑状态和相机画面"));
    });
    connect(_liveMonitorWidget, &LiveMonitorWidget::runOnceRequested, this, [this] {
        executeInspection(TriggerSource::Manual);
    });
    connect(_liveMonitorWidget, &LiveMonitorWidget::runToolRequested, this, [this](const QString& id) {
        if (_inspection->isRunning()) {
            _statusLabel->setText(QStringLiteral("上一轮检测尚未结束，请稍后测试"));
            return;
        }
        executeInspection(TriggerSource::Manual, id);
    });
    connect(_liveMonitorWidget, &LiveMonitorWidget::pausedChanged, this, [this](bool paused) {
        _coordinator->setPaused(paused);
        _statusLabel->setText(_inspection->requiresRecovery() ? _inspection->recoveryReason()
            : paused ? QStringLiteral("检测已暂停") : QStringLiteral("检测已恢复"));
    });
    connect(
        &SchemeCommunicationRuntime::instance(), &SchemeCommunicationRuntime::statusChanged, this, [this] {
            const QString summary = SchemeCommunicationRuntime::instance().summary();
            _communicationStatus->setText(summary);
            _communicationStatus->setToolTip(summary);
        });
    QString catalogError;
    if (!_schemeCatalog.load(&catalogError))
        _statusLabel->setText(QStringLiteral("方案目录读取失败：%1").arg(catalogError));
    refreshSchemeControl();
    applyTriggerSettings();
}

void WorkbenchWidget::switchTopModule(int id)
{
    if (id == 0 && !_editingSchemeName.isEmpty()) {
        _topStack->setCurrentIndex(2);
        _statusLabel->setText(QStringLiteral("请保存或取消当前方案编辑，完成后恢复检测"));
        return;
    }
    const int stackIndex = id == 0 ? 0 : 1;
    _topStack->setCurrentIndex(stackIndex);
}

void WorkbenchWidget::switchWorkflowStep(int id)
{
    if (_workflowStack)
        _workflowStack->setCurrentIndex(id);
    if (_statusLabel)
        _statusLabel->setText(QStringLiteral("编辑方案  |  当前步骤：%1")
                .arg(_stepGroup->button(id)->property("stepTitle").toString()));
}

void WorkbenchWidget::setActiveStep(int id)
{
    if (_stepGroup && _stepGroup->button(id))
        _stepGroup->button(id)->setChecked(true);
    switchWorkflowStep(id);
}

void WorkbenchWidget::refreshSchemeControl()
{
    _liveMonitorWidget->setSchemes(_schemeCatalog.schemeNames(), _schemeCatalog.currentSchemeName());
    const QString current = _schemeCatalog.currentSchemeName();
    _liveMonitorWidget->setCurrentSchemeTools(_schemeCatalog.tools(current));
    _planLabel->setText(
        current.isEmpty() ? QStringLiteral("尚无已保存方案") : QStringLiteral("当前方案：%1").arg(current));
}

WorkbenchWidget::~WorkbenchWidget()
{
    _coordinator->shutdown();
}
