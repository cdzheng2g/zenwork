#include "CameraSettingsPage.h"
#include "ui/components/NumericParameterField.h"

#include "ImageCanvasWidget.h"
#include "infrastructure/camera/CameraController.h"
#include "infrastructure/camera/CameraParamsStore.h"
#include "infrastructure/camera/CameraStreamService.h"

#include "ElaMessageBar.h"
#include "ElaScrollArea.h"
#include "ElaText.h"

#include <QButtonGroup>
#include <QDoubleSpinBox>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSizePolicy>
#include <QSlider>
#include <QTimer>
#include <QVBoxLayout>

CameraSettingsPage::CameraSettingsPage(QWidget* parent)
    : QWidget(parent)
{
    initContent();
    CameraParams params;
    if (CameraParamsStore::instance().load(QStringLiteral("default"), params))
        _statusText->setText(QStringLiteral("已加载默认相机参数"));
    else
        _statusText->setText(QStringLiteral("使用出厂默认相机参数"));
    loadParamsIntoUi(params);
}

CameraSettingsPage::~CameraSettingsPage()
{
    stopPreview();
}

void CameraSettingsPage::startPreview()
{
    if (_cameraLeaseHeld)
        return;

    setPreviewStatus(QStringLiteral("连接中"), false);
    CameraStreamService::instance().acquire();
    _cameraLeaseHeld = true;
}

void CameraSettingsPage::stopPreview()
{
    if (!_cameraLeaseHeld)
        return;

    CameraStreamService::instance().release();
    _cameraLeaseHeld = false;
    setPreviewStatus(QStringLiteral("已暂停"), false);
}

void CameraSettingsPage::onFrameReady(const cv::Mat& frame)
{
    _previewCanvas->setImage(frame);
    if (_previewStatus->text() != QStringLiteral("●  采集中"))
        setPreviewStatus(QStringLiteral("采集中"), true);
}

void CameraSettingsPage::onStreamError(const QString& message)
{
    // 取流失败不弹打断式对话框,只在状态栏提示,避免无摄像头的本机调试反复弹窗。
    _statusText->setText(QStringLiteral("预览: %1").arg(message));
    setPreviewStatus(QStringLiteral("取流失败"), false);
}

void CameraSettingsPage::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    startPreview();
}

void CameraSettingsPage::hideEvent(QHideEvent* event)
{
    stopPreview();
    QWidget::hideEvent(event);
}

CameraSettingsPage::ParamRow CameraSettingsPage::addParamRow(QVBoxLayout* layout, const QString& label,
    double minValue, double maxValue, double scale, int decimals, const QString& suffix)
{
    auto* labelText = new QLabel(label, this);
    labelText->setStyleSheet("color:#253041;font-size:14px;font-weight:600;");
    layout->addWidget(labelText);
    auto* field = new NumericParameterField(
        numberParameter({ }, label, minValue, minValue, maxValue, decimals, 1.0 / scale, suffix, true), this);
    layout->addWidget(field);
    layout->addSpacing(5);
    return { field->slider(), field->spinBox() };
}

void CameraSettingsPage::initContent()
{
    auto* rootLayout = new QHBoxLayout(this);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(14);

    auto* parameterScrollArea = new ElaScrollArea(this);
    parameterScrollArea->setFixedWidth(426);
    parameterScrollArea->setWidgetResizable(true);
    parameterScrollArea->setIsGrabGesture(true, 0.12);
    parameterScrollArea->setIsOverShoot(Qt::Vertical, true);
    parameterScrollArea->setStyleSheet(
        QStringLiteral("QScrollArea{background:transparent;border:0;}"
                       "QScrollArea>QWidget>QWidget{background:transparent;}"));

    auto* panel = new QFrame(parameterScrollArea);
    panel->setObjectName(QStringLiteral("cameraControlCard"));
    panel->setStyleSheet(QStringLiteral(
        "QFrame#cameraControlCard{background:#fff;border:1px solid #dce2e8;border-radius:12px;}"));
    auto* panelLayout = new QVBoxLayout(panel);
    panelLayout->setContentsMargins(16, 16, 16, 16);
    panelLayout->setSpacing(5);

    auto* titleText = new QLabel(QStringLiteral("曝光控制"), panel);
    titleText->setStyleSheet(QStringLiteral("font-size:18px;font-weight:600;color:#1f2937;"));
    titleText->setFixedHeight(36);
    panelLayout->addWidget(titleText);

    auto* modeRow = new QHBoxLayout();
    auto* modeLabel = new QLabel(QStringLiteral("曝光模式"), panel);
    modeLabel->setStyleSheet(QStringLiteral("font-size:13px;color:#505c6b;"));
    modeRow->addWidget(modeLabel);
    modeRow->addSpacing(15);
    _autoExposureButton = new QPushButton(QStringLiteral("自动"), panel);
    _manualExposureButton = new QPushButton(QStringLiteral("手动"), panel);
    for (auto* button : { _autoExposureButton, _manualExposureButton }) {
        button->setCheckable(true);
        button->setMinimumHeight(42);
        button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        modeRow->addWidget(button, 1);
    }
    auto* modeGroup = new QButtonGroup(this);
    modeGroup->setExclusive(true);
    modeGroup->addButton(_autoExposureButton);
    modeGroup->addButton(_manualExposureButton);
    connect(_autoExposureButton, &QPushButton::clicked, this, [this] { onAutoExposureToggled(true); });
    connect(_manualExposureButton, &QPushButton::clicked, this, [this] { onAutoExposureToggled(false); });
    panelLayout->addLayout(modeRow);
    panelLayout->addSpacing(6);

    _exposureRow
        = addParamRow(panelLayout, QStringLiteral("曝光时间"), 0.1, 100.0, 10.0, 1, QStringLiteral(" ms"));
    _gainRow
        = addParamRow(panelLayout, QStringLiteral("Sensor 增益"), 1.0, 16.0, 10.0, 1, QStringLiteral(" ×"));
    _ispDGainRow
        = addParamRow(panelLayout, QStringLiteral("ISP 数字增益"), 1.0, 4.0, 10.0, 1, QStringLiteral(" ×"));

    auto* separator = new QFrame(panel);
    separator->setFrameShape(QFrame::HLine);
    separator->setStyleSheet(QStringLiteral("color:#e1e5eb;"));
    panelLayout->addSpacing(5);
    panelLayout->addWidget(separator);
    auto* qualityTitle = new QLabel(QStringLiteral("画质调节"), panel);
    qualityTitle->setStyleSheet(QStringLiteral("font-size:18px;font-weight:600;color:#1f2937;"));
    panelLayout->addWidget(qualityTitle);

    _contrastRow = addParamRow(panelLayout, "对比度", 0, 255, 1.0, 0, "");
    _saturationRow = addParamRow(panelLayout, "饱和度", 0, 255, 1.0, 0, "");
    _sharpnessRow = addParamRow(panelLayout, "锐度", 0, 100, 1.0, 0, " %");

    _statusText = new ElaText("参数修改后实时生效", panel);
    _statusText->setTextPixelSize(12);
    _statusText->setIsWrapAnywhere(true);
    _statusText->setStyleSheet(QStringLiteral("color:#55966f;"));
    panelLayout->addWidget(_statusText);

    auto* buttonRow = new QHBoxLayout();
    buttonRow->setSpacing(10);
    _applyButton = new QPushButton("保存参数", panel);
    _applyButton->setFixedHeight(44);
    _applyButton->setStyleSheet(
        QStringLiteral("QPushButton{background:#b51d32;color:white;border:1px solid #b51d32;"
                       "border-radius:8px;font-size:15px;font-weight:600;}"
                       "QPushButton:hover{background:#9e182a;}"));
    connect(_applyButton, &QPushButton::clicked, this, &CameraSettingsPage::onApplyClicked);
    _resetButton = new QPushButton("恢复默认", panel);
    _resetButton->setFixedHeight(44);
    _resetButton->setStyleSheet(
        QStringLiteral("QPushButton{background:#fff;color:#536071;border:1px solid #dce2e8;"
                       "border-radius:8px;font-size:15px;}"
                       "QPushButton:hover{background:#f5f6f8;}"));
    connect(_resetButton, &QPushButton::clicked, this, &CameraSettingsPage::onResetDefaultClicked);
    buttonRow->addWidget(_resetButton, 1);
    buttonRow->addWidget(_applyButton, 1);
    panelLayout->addLayout(buttonRow);

    auto* previewCard = new QFrame(this);
    previewCard->setObjectName(QStringLiteral("cameraPreviewCard"));
    previewCard->setStyleSheet(QStringLiteral(
        "QFrame#cameraPreviewCard{background:#fff;border:1px solid #dce2e8;border-radius:12px;}"));
    auto* previewLayout = new QVBoxLayout(previewCard);
    previewLayout->setContentsMargins(16, 16, 16, 16);
    previewLayout->setSpacing(12);
    auto* previewHeader = new QHBoxLayout();
    auto* previewTitle = new QLabel(QStringLiteral("实时预览"), previewCard);
    previewTitle->setStyleSheet(QStringLiteral("font-size:18px;font-weight:600;color:#1f2937;"));
    _previewStatus = new QLabel(previewCard);
    setPreviewStatus(QStringLiteral("待连接"), false);
    previewTitle->setFixedHeight(36);
    previewHeader->setSpacing(12);
    previewHeader->addWidget(previewTitle);
    previewHeader->addStretch(1);
    previewHeader->addWidget(_previewStatus);
    previewLayout->addLayout(previewHeader);
    _previewCanvas = new ImageCanvasWidget(previewCard);
    _previewCanvas->setMinimumSize(320, 240);
    _previewCanvas->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    previewLayout->addWidget(_previewCanvas, 1);

    parameterScrollArea->setWidget(panel);
    rootLayout->addWidget(parameterScrollArea);
    rootLayout->addWidget(previewCard, 1);

    _realtimeApplyTimer = new QTimer(this);
    _realtimeApplyTimer->setSingleShot(true);
    connect(_realtimeApplyTimer, &QTimer::timeout, this, &CameraSettingsPage::applyRealtimeParams);

    const auto connectRealtimeRow = [this](const ParamRow& row) {
        connect(row.slider, &QSlider::valueChanged, this, &CameraSettingsPage::scheduleRealtimeApply);
        connect(row.spinBox, qOverload<double>(&QDoubleSpinBox::valueChanged), this,
            &CameraSettingsPage::scheduleRealtimeApply);
    };
    connectRealtimeRow(_exposureRow);
    connectRealtimeRow(_gainRow);
    connectRealtimeRow(_ispDGainRow);
    connectRealtimeRow(_contrastRow);
    connectRealtimeRow(_saturationRow);
    connectRealtimeRow(_sharpnessRow);

    connect(&CameraStreamService::instance(), &CameraStreamService::frameReady, this,
        &CameraSettingsPage::onFrameReady);
    connect(&CameraStreamService::instance(), &CameraStreamService::streamError, this,
        &CameraSettingsPage::onStreamError);
}

void CameraSettingsPage::loadParamsIntoUi(const CameraParams& params)
{
    _loadingParams = true;
    (params.autoExposure ? _autoExposureButton : _manualExposureButton)->setChecked(true);
    updateExposureModeStyle();

    _exposureRow.spinBox->setValue(params.exposureMs);
    _gainRow.spinBox->setValue(params.gain);
    _ispDGainRow.spinBox->setValue(params.ispDGain);
    _contrastRow.spinBox->setValue(params.contrast);
    _saturationRow.spinBox->setValue(params.saturation);
    _sharpnessRow.spinBox->setValue(params.sharpness);

    setManualControlsEnabled(!params.autoExposure);
    _loadingParams = false;
}

CameraParams CameraSettingsPage::paramsFromUi() const
{
    CameraParams params;
    params.autoExposure = _autoExposureButton->isChecked();
    params.exposureMs = _exposureRow.spinBox->value();
    params.gain = _gainRow.spinBox->value();
    params.ispDGain = _ispDGainRow.spinBox->value();
    params.contrast = static_cast<int>(_contrastRow.spinBox->value());
    params.saturation = static_cast<int>(_saturationRow.spinBox->value());
    params.sharpness = _sharpnessRow.spinBox->value();
    return params;
}

void CameraSettingsPage::setManualControlsEnabled(bool enabled)
{
    _exposureRow.slider->setEnabled(enabled);
    _exposureRow.spinBox->setEnabled(enabled);
    _gainRow.slider->setEnabled(enabled);
    _gainRow.spinBox->setEnabled(enabled);
    _ispDGainRow.slider->setEnabled(enabled);
    _ispDGainRow.spinBox->setEnabled(enabled);
}

void CameraSettingsPage::onAutoExposureToggled(bool checked)
{
    updateExposureModeStyle();
    setManualControlsEnabled(!checked);
    scheduleRealtimeApply();
}

void CameraSettingsPage::updateExposureModeStyle()
{
    const auto style = [](bool selected) {
        return selected
            ? QStringLiteral("QPushButton{background:#fff;color:#b51d32;border:1px solid #e7c8ce;"
                             "border-radius:9px;font-size:15px;font-weight:600;}")
            : QStringLiteral("QPushButton{background:#f2f4f7;color:#778293;border:1px solid #f2f4f7;"
                             "border-radius:9px;font-size:15px;}");
    };
    _autoExposureButton->setStyleSheet(style(_autoExposureButton->isChecked()));
    _manualExposureButton->setStyleSheet(style(_manualExposureButton->isChecked()));
}

void CameraSettingsPage::setPreviewStatus(const QString& text, bool active)
{
    if (!_previewStatus)
        return;
    _previewStatus->setText(QStringLiteral("●  ") + text);
    _previewStatus->setStyleSheet(active
            ? QStringLiteral(
                  "color:#328760;background:#e9f8ef;border-radius:12px;padding:5px 10px;font-size:12px;")
            : QStringLiteral(
                  "color:#778293;background:#f2f4f7;border-radius:12px;padding:5px 10px;font-size:12px;"));
}

void CameraSettingsPage::onApplyClicked()
{
    const CameraParams params = paramsFromUi();
    const bool appliedOk = CameraController::instance().applyParams(params);
    const bool savedOk = CameraParamsStore::instance().save(QStringLiteral("default"), params);

    if (appliedOk && savedOk) {
        ElaMessageBar::success(ElaMessageBarType::Top, "成功", "相机参数已应用并保存", 2000, this);
    } else {
        ElaMessageBar::error(
            ElaMessageBarType::Top, "失败", appliedOk ? "参数已应用,但保存失败" : "参数应用失败", 2000, this);
    }
}

void CameraSettingsPage::scheduleRealtimeApply()
{
    if (_loadingParams || !_realtimeApplyTimer)
        return;

    // 拖动触摸滑块会产生大量valueChanged事件；停下约200ms后再写ISP。
    _realtimeApplyTimer->start(220);
}

void CameraSettingsPage::applyRealtimeParams()
{
    if (!CameraController::instance().isInitialized()) {
        _statusText->setText(QStringLiteral("ISP未就绪，参数尚未下发；请检查AIQ启动日志"));
        return;
    }

    if (CameraController::instance().applyParams(paramsFromUi())) {
        _statusText->setText(QStringLiteral("实时参数已应用；点击“保存参数”以持久化"));
    } else {
        _statusText->setText(QStringLiteral("ISP拒绝参数更新，配置未生效"));
    }
}

void CameraSettingsPage::onResetDefaultClicked()
{
    loadParamsIntoUi(CameraParams());
    scheduleRealtimeApply();
}
