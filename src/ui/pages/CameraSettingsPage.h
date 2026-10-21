#ifndef CAMERASETTINGSPAGE_H
#define CAMERASETTINGSPAGE_H

#include "infrastructure/camera/CameraParams.h"

#include <QWidget>

#include <opencv2/core.hpp>

class ElaText;
class QVBoxLayout;
class QTimer;
class QSlider;
class QDoubleSpinBox;
class QPushButton;
class QLabel;
class ImageCanvasWidget;

class CameraSettingsPage : public QWidget {
    Q_OBJECT
public:
    explicit CameraSettingsPage(QWidget* parent = nullptr);
    ~CameraSettingsPage() override;
    void loadParamsIntoUi(const CameraParams& params);
    CameraParams paramsFromUi() const;

protected:
    // 页面只订阅共享帧流；板端通过 VI->VPSS 采集，再由 Qt 绘制预览。
    void showEvent(QShowEvent* event) override;
    void hideEvent(QHideEvent* event) override;

private Q_SLOTS:
    void onAutoExposureToggled(bool checked);
    void onApplyClicked();
    void onResetDefaultClicked();
    void scheduleRealtimeApply();
    void applyRealtimeParams();
    void onFrameReady(const cv::Mat& frame);
    void onStreamError(const QString& message);

private:
    void startPreview();
    void stopPreview();
    struct ParamRow {
        QSlider* slider { nullptr };
        QDoubleSpinBox* spinBox { nullptr };
    };

    void initContent();
    ParamRow addParamRow(QVBoxLayout* layout, const QString& label, double minValue, double maxValue,
        double scale, int decimals, const QString& suffix);

    void setManualControlsEnabled(bool enabled);
    void updateExposureModeStyle();
    void setPreviewStatus(const QString& text, bool active);

    QPushButton* _autoExposureButton { nullptr };
    QPushButton* _manualExposureButton { nullptr };

    ParamRow _exposureRow;
    ParamRow _gainRow;
    ParamRow _ispDGainRow;
    ParamRow _contrastRow;
    ParamRow _saturationRow;
    ParamRow _sharpnessRow;

    QPushButton* _applyButton { nullptr };
    QPushButton* _resetButton { nullptr };
    ElaText* _statusText { nullptr };

    // 实时预览画布:显示CameraStreamWorker取到的实时帧;无设备/未取到帧时
    // 由ImageCanvasWidget自身显示黑底"无图像"占位。
    ImageCanvasWidget* _previewCanvas { nullptr };
    QLabel* _previewStatus { nullptr };
    QTimer* _realtimeApplyTimer { nullptr };
    bool _loadingParams { false };
    bool _cameraLeaseHeld { false };
};

#endif // CAMERASETTINGSPAGE_H
