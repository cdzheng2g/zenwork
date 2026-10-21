#ifndef WORKBENCHWIDGET_H
#define WORKBENCHWIDGET_H

#include <QWidget>
#include <functional>
#include "application/inspection/InspectionService.h"
#include <QJsonObject>
#include <QMap>
#include <opencv2/core.hpp>
class QComboBox;

#include "../../application/scheme/SchemeCatalog.h"

class QButtonGroup;
class QStackedWidget;
class QLabel;
class QToolButton;
class QVBoxLayout;
class CameraSettingsPage;
class ReferenceManagePage;
class CommunicationSettingsPage;
class AlgorithmConfigPage;
class LiveMonitorWidget;
class InternalTrigger;
class QCheckBox;
class QSpinBox;
class QLineEdit;

// 工作台负责页面导航与共享状态；布局、方案、文件和检测逻辑分别在同目录的子目录中。
class WorkbenchWidget : public QWidget {
    Q_OBJECT
public:
    explicit WorkbenchWidget(QWidget* parent = nullptr);
    ~WorkbenchWidget() override;

private Q_SLOTS:
    void switchTopModule(int id);
    void switchWorkflowStep(int id);
    void createScheme();
    void editCurrentScheme();
    void selectScheme(const QString& schemeName);
    void saveEditingScheme();
    void cancelEditingScheme();
    void deleteCurrentScheme();
    void showSchemeFilePanel(bool exporting);

private:
    void importSchemeFile(const QString& path, std::function<void(const QString&)> finished);
    void exportSchemeFile(
        const QString& path, bool includeModels, std::function<void(const QString&)> finished);
    void applyCurrentSchemeCamera();

protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    QWidget* _filePanel { nullptr };
    static QString panelStyle();
    QWidget* createTopBar();
    QWidget* createWorkflowWorkspace();
    QWidget* createOutputWorkspace();
    QWidget* createTriggerWorkspace();
    QWidget* createLogicWorkspace();
    QWidget* createStatusBar();
    QWidget* createStepButton(const QString& title, const QString& detail, int id);
    QWidget* createPlaceholder(const QString& title, const QString& detail);
    void setActiveStep(int id);
    void refreshSchemeControl();
    void beginEditingScheme(const QString& schemeName, const SchemeRuntimeSettings& settings);
    void applyTriggerSettings();
    void refreshLogicPreview();
    bool executeInspection(TriggerSource source = TriggerSource::Internal, const QString& toolId = { });
    InspectionGate inspectionGate() const;
    void completeInspection();
    void handleModbusTrigger();
    bool modbusTriggerReady() const;
    void refreshModbusState();
    InspectionService* _inspection { nullptr };
    QComboBox* _logicRule { nullptr };

    QButtonGroup* _topGroup { nullptr };
    QButtonGroup* _stepGroup { nullptr };
    QStackedWidget* _topStack { nullptr };
    QStackedWidget* _workflowStack { nullptr };
    QLabel* _statusLabel { nullptr };
    QLabel* _planLabel { nullptr };
    QLabel* _communicationStatus { nullptr };
    bool _communicationReady { false };
    QLabel* _workflowTitle { nullptr };
    QLabel* _logicPreview { nullptr };
    QLineEdit* _schemeNameEdit { nullptr };
    QComboBox* _triggerMode { nullptr };
    QSpinBox* _triggerIntervalMs { nullptr };

    CameraSettingsPage* _cameraSettingsPage { nullptr };
    ReferenceManagePage* _referenceManagePage { nullptr };
    CommunicationSettingsPage* _communicationSettingsPage { nullptr };
    AlgorithmConfigPage* _algorithmConfigPage { nullptr };
    LiveMonitorWidget* _liveMonitorWidget { nullptr };
    SchemeCatalog _schemeCatalog;
    SchemeRuntimeSettings _editingSettings;
    QString _editingSchemeName;
    QList<SchemeTool> _editingTools;
    InternalTrigger* _internalTrigger { nullptr };
};

#endif // WORKBENCHWIDGET_H
