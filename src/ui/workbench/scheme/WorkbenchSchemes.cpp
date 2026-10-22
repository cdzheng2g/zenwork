#include "ui/workbench/WorkbenchWidget.h"
#include "ui/workbench/LiveMonitorWidget.h"
#include "ui/FileOperation.h"
#include "ui/pages/AlgorithmConfigPage.h"
#include "ui/pages/CameraSettingsPage.h"
#include "ui/pages/ReferenceManagePage.h"
#include "ui/pages/CommunicationSettingsPage.h"
#include "application/communication/SchemeCommunicationRuntime.h"
#include "app/CreateInspectionCoordinator.h"
#include "application/scheme/SchemePackage.h"
#include "infrastructure/camera/CameraController.h"
#include "infrastructure/camera/CameraParamsJson.h"
#include "infrastructure/reference/ReferenceImageManager.h"

#include <QComboBox>
#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QSpinBox>
#include <QStackedWidget>
#include <QStringList>

void WorkbenchWidget::createScheme()
{
    int index = 1;
    QString name;
    do {
        name = QStringLiteral("新方案 %1").arg(index++);
    } while (_schemeCatalog.schemeNames().contains(name));
    beginEditingScheme(name, SchemeRuntimeSettings { });
}

void WorkbenchWidget::editCurrentScheme()
{
    const QString current = _schemeCatalog.currentSchemeName();
    if (current.isEmpty()) {
        _statusLabel->setText(QStringLiteral("请先新建并保存方案"));
        return;
    }
    beginEditingScheme(current, _schemeCatalog.settings(current));
}

void WorkbenchWidget::selectScheme(const QString& schemeName)
{
    if (_schemeCatalog.select(schemeName)) {
        applyTriggerSettings();
        refreshSchemeControl();
    }
}

void WorkbenchWidget::beginEditingScheme(const QString& schemeName, const SchemeRuntimeSettings& settings)
{
    _coordinator->setEditing(true);
    _editingSchemeName = schemeName;
    _editingSettings = settings;
    if (!settings.camera.isEmpty())
        _cameraSettingsPage->loadParamsIntoUi(cameraParamsFromJson(settings.camera));
    _logicRule->setCurrentIndex(settings.logicOr ? 1 : 0);
    _referenceManagePage->selectModel(settings.referenceModel);
    _editingTools = _schemeCatalog.tools(schemeName);
    _algorithmConfigPage->setConfiguredTools(_editingTools);
    refreshLogicPreview();
    _workflowTitle->setText(QStringLiteral("编辑方案  /  %1").arg(schemeName));
    _schemeNameEdit->setText(schemeName);
    _triggerMode->setCurrentIndex(settings.modbusTriggerEnabled ? 1
            : settings.internalTriggerEnabled                   ? 0
                                                                : 2);
    _communicationSettingsPage->setSettings(settings);
    _triggerIntervalMs->setEnabled(settings.internalTriggerEnabled);
    _triggerIntervalMs->setValue(settings.triggerIntervalMs);
    _topStack->setCurrentIndex(2);
    setActiveStep(0);
    _statusLabel->setText(QStringLiteral("正在编辑方案：%1（修改后请点击保存方案）").arg(schemeName));
}

void WorkbenchWidget::saveEditingScheme()
{
    _editingSchemeName = _schemeNameEdit->text().trimmed();
    if (_editingSchemeName.isEmpty()) {
        _statusLabel->setText(QStringLiteral("没有待保存的方案"));
        return;
    }
    _editingSettings.logicOr = _logicRule->currentIndex() == 1;
    _editingSettings.referenceModel = ReferenceImageManager::instance().currentModel();
    _editingTools = _algorithmConfigPage->configuredTools();
    _editingSettings.camera = cameraParamsJson(_cameraSettingsPage->paramsFromUi());
    _editingSettings.internalTriggerEnabled = _triggerMode->currentIndex() == 0;
    _editingSettings.modbusTriggerEnabled = _triggerMode->currentIndex() == 1;
    _editingSettings.outputChannels = _communicationSettingsPage->outputChannels();
    _editingSettings.communication = _communicationSettingsPage->configuration();
    const QString communicationError = validateCommunicationSettings(_editingSettings.communication,
        _editingSettings.outputChannels, _editingSettings.modbusTriggerEnabled, true);
    if (!communicationError.isEmpty()) {
        setActiveStep(5);
        _statusLabel->setText(communicationError);
        return;
    }
    _editingSettings.triggerIntervalMs = _triggerIntervalMs->value();
    SchemePackage package;
    package.name = _editingSchemeName;
    package.settings = _editingSettings;
    package.tools = _editingTools;
    if (const auto* ref = ReferenceImageManager::instance().referenceImage(_editingSettings.referenceModel))
        package.reference = *ref;

    const QString path = _schemeCatalog.portablePath(package.name);
    _coordinator->setFileBusy(true);
    const bool submitted = FileOperation::run(
        this, QStringLiteral("正在保存方案和资源"),
        [package, path] {
            QString error;
            package.save(path, &error);
            return error;
        },
        [this, package, path](const QString& failure) {
            _coordinator->setFileBusy(false);
            QString error = failure;
            if (!error.isEmpty()
                || !_schemeCatalog.saveComplete(package.name, package.settings, package.tools, &error)) {
                _statusLabel->setText(QStringLiteral("方案保存失败：%1").arg(error));
                return;
            }
            _statusLabel->setText(QStringLiteral("已保存 YAML：%1").arg(path));
            _statusLabel->setToolTip(path);
            _editingSchemeName.clear();
            applyTriggerSettings();
            refreshSchemeControl();
            _topStack->setCurrentIndex(0);
        });
    if (!submitted) {
        _coordinator->setFileBusy(false);
        _statusLabel->setText(QStringLiteral("已有文件操作正在进行，请稍后保存"));
    }
}

void WorkbenchWidget::cancelEditingScheme()
{
    _editingSchemeName.clear();
    _topStack->setCurrentIndex(0);
    _statusLabel->setText(QStringLiteral("已取消未保存的编辑"));
    applyTriggerSettings();
}

void WorkbenchWidget::applyTriggerSettings()
{
    _coordinator->setMode({!_editingSchemeName.isEmpty(), _filePanel != nullptr, _liveMonitorWidget->isPaused()});
    if (!_editingSchemeName.isEmpty() || _filePanel) return;
    QString error;
    if (!activateCatalogScheme(*_coordinator, _schemeCatalog, &error))
        _statusLabel->setText(error);
    const auto current = _schemeCatalog.currentSchemeName();
    const auto settings = _schemeCatalog.settings(current);
    if (!settings.camera.isEmpty())
        _cameraSettingsPage->loadParamsIntoUi(cameraParamsFromJson(settings.camera));
    _liveMonitorWidget->setTriggerSettings(
        settings.internalTriggerEnabled, settings.triggerIntervalMs, settings.modbusTriggerEnabled);
}

void WorkbenchWidget::refreshLogicPreview()
{
    if (!_logicPreview)
        return;
    if (_editingTools.isEmpty()) {
        _logicPreview->setText(QStringLiteral("相机输入  →  请先添加工具  →  最终判定"));
        return;
    }
    QStringList toolNames;
    for (const SchemeTool& tool : _editingTools)
        toolNames.append(QStringLiteral("[%1]").arg(tool.name));
    _logicPreview->setText(QStringLiteral("相机输入  →  %1  →  AND/OR 最终判定  →  方案输出")
            .arg(toolNames.join(QStringLiteral("  +  "))));
}

void WorkbenchWidget::deleteCurrentScheme()
{
    const QString current = _schemeCatalog.currentSchemeName();
    if (current.isEmpty())
        return;
    const auto answer = QMessageBox::question(this, QStringLiteral("删除方案"),
        QStringLiteral("确认删除方案“%1”？此操作不能撤销。").arg(current),
        QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Cancel);
    if (answer != QMessageBox::Yes)
        return;
    QString error;
    if (!_schemeCatalog.remove(current, &error)) {
        _statusLabel->setText(QStringLiteral("删除方案失败：%1").arg(error));
        return;
    }
    applyTriggerSettings();
    refreshSchemeControl();
    _statusLabel->setText(QStringLiteral("方案已删除"));
}
