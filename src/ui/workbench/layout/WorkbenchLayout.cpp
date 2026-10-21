#include "ui/workbench/WorkbenchWidget.h"
#include "ui/pages/AlgorithmConfigPage.h"
#include "ui/pages/CameraSettingsPage.h"
#include "ui/pages/CommunicationSettingsPage.h"
#include "application/communication/ResultOutputDispatcher.h"
#include "ui/pages/ReferenceManagePage.h"

#include <QButtonGroup>
#include <QCheckBox>
#include <QComboBox>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSizePolicy>
#include <QSpinBox>
#include <QStackedWidget>
#include <QStringList>
#include <QToolButton>
#include <QVBoxLayout>

namespace {
QLabel* makeLabel(const QString& text, const QString& objectName = { })
{
    auto* label = new QLabel(text);
    if (!objectName.isEmpty())
        label->setObjectName(objectName);
    return label;
}
} // namespace

QWidget* WorkbenchWidget::createTopBar()
{
    auto* bar = new QFrame(this);
    bar->setObjectName("topBar");
    bar->setFixedHeight(58);
    auto* layout = new QHBoxLayout(bar);
    layout->setContentsMargins(20, 0, 12, 0);
    layout->setSpacing(4);

    auto* brand = makeLabel(QStringLiteral("HANS.AI  智能相机"));
    brand->setStyleSheet("font-size:18px;font-weight:600;color:#ffffff;");
    layout->addWidget(brand);
    layout->addSpacing(22);

    _topGroup = new QButtonGroup(this);
    _topGroup->setExclusive(true);
    const QStringList titles { QStringLiteral("方案主控"), QStringLiteral("系统设置") };
    for (int i = 0; i < titles.size(); ++i) {
        auto* button = new QToolButton(bar);
        button->setObjectName("topButton");
        button->setText(titles.at(i));
        button->setCheckable(true);
        button->setFixedHeight(58);
        _topGroup->addButton(button, i);
        layout->addWidget(button);
    }
    connect(_topGroup, &QButtonGroup::idClicked, this, &WorkbenchWidget::switchTopModule);
    layout->addStretch(1);

    return bar;
}

QWidget* WorkbenchWidget::createWorkflowWorkspace()
{
    auto* page = new QWidget(this);
    auto* layout = new QHBoxLayout(page);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    auto* stepBar = new QFrame(page);
    stepBar->setObjectName("stepBar");
    stepBar->setFixedWidth(176);
    auto* stepLayout = new QVBoxLayout(stepBar);
    stepLayout->setContentsMargins(8, 16, 8, 16);
    stepLayout->setSpacing(4);
    stepLayout->addWidget(makeLabel(QStringLiteral("方案配置"), "sectionTitle"));
    stepLayout->addSpacing(12);

    _stepGroup = new QButtonGroup(this);
    _stepGroup->setExclusive(true);
    const QList<QPair<QString, QString>> steps { { QStringLiteral("相机参数"),
                                                     QStringLiteral("曝光、增益、画质") },
        { QStringLiteral("基准图"), QStringLiteral("选择图像和定位区域") },
        { QStringLiteral("算子配置"), QStringLiteral("添加与编辑") },
        { QStringLiteral("逻辑判定"), QStringLiteral("AND/OR与结果规则") },
        { QStringLiteral("触发配置"), QStringLiteral("检测触发方式") },
        { QStringLiteral("通信与输出"), QStringLiteral("协议与输出参数") } };
    for (int i = 0; i < steps.size(); ++i) {
        auto* button = qobject_cast<QToolButton*>(createStepButton(steps.at(i).first, steps.at(i).second, i));
        _stepGroup->addButton(button, i);
        stepLayout->addWidget(button);
    }
    stepLayout->addStretch(1);
    connect(_stepGroup, &QButtonGroup::idClicked, this, &WorkbenchWidget::switchWorkflowStep);

    auto* content = new QWidget(page);
    auto* contentLayout = new QVBoxLayout(content);
    contentLayout->setContentsMargins(18, 16, 18, 16);
    contentLayout->setSpacing(12);

    auto* editHeader = new QHBoxLayout();
    editHeader->setContentsMargins(0, 0, 0, 0);
    editHeader->setSpacing(12);

    _workflowTitle = makeLabel(QStringLiteral("编辑方案"), "sectionTitle");
    editHeader->addWidget(_workflowTitle);
    editHeader->setAlignment(_workflowTitle, Qt::AlignVCenter);
    _schemeNameEdit = new QLineEdit(content);
    _schemeNameEdit->setPlaceholderText(QStringLiteral("方案名称"));
    _schemeNameEdit->setFixedWidth(180);
    _schemeNameEdit->setFixedHeight(36);
    editHeader->addWidget(_schemeNameEdit);
    editHeader->addStretch(1);
    auto* cancelButton = new QPushButton(QStringLiteral("取消"), content);
    cancelButton->setFixedSize(88, 36);
    connect(cancelButton, &QPushButton::clicked, this, &WorkbenchWidget::cancelEditingScheme);
    editHeader->addWidget(cancelButton);
    auto* saveButton = new QPushButton(QStringLiteral("保存方案"), content);
    saveButton->setFixedSize(104, 36);
    connect(saveButton, &QPushButton::clicked, this, &WorkbenchWidget::saveEditingScheme);
    editHeader->addWidget(saveButton);
    contentLayout->addLayout(editHeader);

    _workflowStack = new QStackedWidget(content);
    _cameraSettingsPage = new CameraSettingsPage(_workflowStack);
    _referenceManagePage = new ReferenceManagePage(_workflowStack);
    _algorithmConfigPage = new AlgorithmConfigPage(_workflowStack);
    connect(_algorithmConfigPage, &AlgorithmConfigPage::configuredToolsChanged, this,
        [this](const QList<SchemeTool>& tools) {
            _editingTools = tools;
            refreshLogicPreview();
        });
    _workflowStack->addWidget(_cameraSettingsPage);
    _workflowStack->addWidget(_referenceManagePage);
    _workflowStack->addWidget(_algorithmConfigPage);
    _workflowStack->addWidget(createLogicWorkspace());
    _workflowStack->addWidget(createTriggerWorkspace());
    _workflowStack->addWidget(createOutputWorkspace());

    contentLayout->addWidget(_workflowStack, 1);
    layout->addWidget(stepBar);
    layout->addWidget(content, 1);
    _stepGroup->button(0)->setChecked(true);
    return page;
}

QWidget* WorkbenchWidget::createOutputWorkspace()
{
    _communicationSettingsPage = new CommunicationSettingsPage(this);
    return _communicationSettingsPage;
}

QWidget* WorkbenchWidget::createTriggerWorkspace()
{
    auto* page = new QFrame(this);
    page->setObjectName("configPanel");
    auto* layout = new QVBoxLayout(page);
    layout->setContentsMargins(16, 16, 16, 16);
    layout->setSpacing(12);
    layout->addWidget(makeLabel(QStringLiteral("触发配置"), "sectionTitle"));
    layout->addWidget(makeLabel(QStringLiteral("设置当前方案何时开始一次检测。"), "muted"));
    layout->addWidget(makeLabel(QStringLiteral("触发方式")));
    _triggerMode = new QComboBox(page);
    _triggerMode->addItems(
        { QStringLiteral("内部触发"), QStringLiteral("Modbus TCP 触发"), QStringLiteral("手动触发") });
    _triggerMode->setFixedSize(280, 36);
    layout->addWidget(_triggerMode);
    auto* row = new QHBoxLayout();
    row->addWidget(makeLabel(QStringLiteral("触发周期")));
    _triggerIntervalMs = new QSpinBox(page);
    _triggerIntervalMs->setRange(50, 60000);
    _triggerIntervalMs->setSingleStep(50);
    _triggerIntervalMs->setSuffix(QStringLiteral(" ms"));
    _triggerIntervalMs->setValue(500);
    _triggerIntervalMs->setFixedSize(160, 36);
    row->addWidget(_triggerIntervalMs);
    row->addStretch(1);
    layout->addLayout(row);
    row->setSpacing(12);
    auto* hint = makeLabel({ }, "muted");
    hint->setWordWrap(true);
    layout->addWidget(hint);
    const auto updateMode = [this, hint](int mode) {
        _triggerIntervalMs->setEnabled(mode == 0);
        hint->setText(mode == 0 ? QStringLiteral("按周期检测；检测中收到的触发不排队。")
                : mode == 1 ? QStringLiteral("通信参数在本方案的“通信与输出”中设置。\n主控运行后，保持寄存器 "
                                             "256 的 bit 0 从 0→1 时检测一次。")
                            : QStringLiteral("在主控页点击“单次检测”。"));
        if (_communicationSettingsPage) {
            _communicationSettingsPage->setModbusTriggerRequired(mode == 1);
            if (mode == 1)
                _communicationSettingsPage->enableOutput(QStringLiteral("modbus_tcp_server"));
        }
    };
    connect(_triggerMode, qOverload<int>(&QComboBox::currentIndexChanged), this, updateMode);
    updateMode(0);
    layout->addStretch(1);
    return page;
}

QWidget* WorkbenchWidget::createLogicWorkspace()
{
    auto* page = new QFrame(this);
    page->setObjectName("configPanel");
    auto* layout = new QVBoxLayout(page);
    layout->setContentsMargins(16, 16, 16, 16);
    layout->setSpacing(12);
    layout->addWidget(makeLabel(QStringLiteral("逻辑判定"), "sectionTitle"));
    layout->addWidget(makeLabel(QStringLiteral("方案结果规则")));
    auto* result = _logicRule = new QComboBox(page);
    _logicRule->setFixedHeight(36);
    result->addItems({ QStringLiteral("AND：所有工具均OK"), QStringLiteral("OR：任一工具OK") });
    layout->addWidget(result);
    layout->addWidget(makeLabel(QStringLiteral("当前流程")));
    _logicPreview = makeLabel(QStringLiteral("相机输入  →  请先添加工具  →  最终判定"), "muted");
    _logicPreview->setWordWrap(true);
    _logicPreview->setMinimumHeight(86);
    layout->addWidget(_logicPreview);
    layout->addWidget(makeLabel(QStringLiteral("在“算子配置”中添加或编辑算子。"), "muted"));
    layout->addStretch(1);
    return page;
}

QWidget* WorkbenchWidget::createStatusBar()
{
    auto* bar = new QFrame(this);
    bar->setObjectName("statusBar");
    bar->setFixedHeight(32);
    auto* layout = new QHBoxLayout(bar);
    layout->setContentsMargins(16, 0, 16, 0);
    _statusLabel = makeLabel(QStringLiteral("就绪  |  方案未启用"), "muted");
    _planLabel = makeLabel(QStringLiteral("采集：未启动    结果：--    耗时：-- ms"), "muted");
    _statusLabel->setMinimumWidth(0);
    _statusLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
    layout->addWidget(_statusLabel, 1);
    layout->addStretch(1);
    _communicationStatus = makeLabel(QStringLiteral("通信：未使用"), "muted");
    _communicationStatus->setMaximumWidth(400);
    layout->addWidget(_communicationStatus);
    layout->addWidget(_planLabel);
    return bar;
}

QWidget* WorkbenchWidget::createStepButton(const QString& title, const QString& detail, int id)
{
    auto* button = new QToolButton(this);
    button->setObjectName("stepButton");
    button->setCheckable(true);
    button->setAutoRaise(true);
    button->setFixedWidth(160);
    button->setFixedHeight(62);
    button->setProperty("workflowId", id);
    button->setProperty("stepTitle", title);
    button->setAccessibleName(title);
    auto* textLayout = new QVBoxLayout(button);
    textLayout->setContentsMargins(15, 5, 10, 5);
    textLayout->setSpacing(1);
    auto* titleLabel = new QLabel(title, button);
    auto* detailLabel = new QLabel(detail, button);
    for (auto* label : { titleLabel, detailLabel }) {
        label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        label->setAttribute(Qt::WA_TransparentForMouseEvents);
        textLayout->addWidget(label);
    }
    const auto updateTextColor = [titleLabel, detailLabel](bool checked) {
        const QString color = checked ? QStringLiteral("#a91427") : QStringLiteral("#3a4653");
        titleLabel->setStyleSheet(QStringLiteral("color:%1;font-size:14px;font-weight:600;").arg(color));
        detailLabel->setStyleSheet(QStringLiteral("color:%1;font-size:12px;").arg(color));
    };
    connect(button, &QToolButton::toggled, button, updateTextColor);
    updateTextColor(false);
    return button;
}

QWidget* WorkbenchWidget::createPlaceholder(const QString& title, const QString& detail)
{
    auto* frame = new QFrame(this);
    frame->setObjectName("imagePanel");
    auto* layout = new QVBoxLayout(frame);
    layout->setContentsMargins(18, 16, 18, 16);
    layout->addWidget(makeLabel(title, "sectionTitle"));
    layout->addSpacing(6);
    layout->addWidget(makeLabel(detail, "placeholder"));
    layout->addStretch(1);
    return frame;
}

QString WorkbenchWidget::panelStyle()
{
    return QStringLiteral(
        "QWidget#workbenchRoot { background:#f5f3f0; color:#202833; font-family:'Noto Sans CJK SC'; }"
        "QFrame#topBar { background:#a91427; border:0; }"
        "QFrame#stepBar { background:#f8f7f5; border-right:1px solid #d7d9dc; }"
        "QFrame#configPanel { background:#ffffff; border:1px solid #d8dcdf; border-radius:4px; }"
        "QFrame#imagePanel { background:#e8e9e8; border:1px solid #c8cdd1; border-radius:3px; }"
        "QFrame#statusBar { background:#ffffff; border-top:1px solid #d8dcdf; }"
        "QToolButton#topButton { color:#fff7f6; border:0; padding:0 16px; font-size:14px; }"
        "QToolButton#topButton:hover { background:#8e1021; }"
        "QToolButton#topButton:checked { color:#ffffff; background:#7e0c1b; border-bottom:3px solid #3b9ced; "
        "}"
        "QToolButton#stepButton { border:0; border-left:3px solid transparent; padding:0; }"
        "QToolButton#stepButton:hover { background:#eceff1; }"
        "QToolButton#stepButton:checked { color:#a91427; background:#f6e9ea; border-left:3px solid #a91427; }"
        "QLabel#muted { color:#71808c; }"
        "QLabel#sectionTitle { color:#202833; font-size:18px; font-weight:600; }"
        "QLabel#placeholder { color:#71808c; font-size:14px; }"
        "QPushButton { min-height:34px; padding:0 15px; color:#7e0c1b; background:#ffffff; border:1px solid "
        "#b72b3b; border-radius:3px; font-weight:600; }"
        "QPushButton:hover { background:#f9e9ea; }"
        "QListWidget { border:0; background:transparent; color:#35414d; outline:0; }"
        "QListWidget::item { padding:14px 10px; border-bottom:1px solid #e0e3e5; }"
        "QListWidget::item:selected { color:#a91427; background:#f8eded; border-left:3px solid #a91427; }");
}
