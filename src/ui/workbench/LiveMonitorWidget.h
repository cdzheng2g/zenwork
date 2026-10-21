#ifndef LIVEMONITORWIDGET_H
#define LIVEMONITORWIDGET_H

#include <opencv2/core.hpp>

#include "../../application/scheme/SchemeCatalog.h"
#include "infrastructure/system/SystemResourceMonitor.h"

#include <QWidget>
#include <QJsonObject>

class QLabel;
class QListWidget;
class QPushButton;
class QTableWidget;
class ImageCanvasWidget;
class ImageInputPanel;
class QComboBox;
class QCheckBox;
class InspectionTrendWidget;
class QTimer;

// 运行监控工作区:实时图像为视觉中心,右侧只显示少量关键状态和结果摘要,
// 底部提供触摸友好的采集操作入口。取流由全局 CameraStreamService 统一提供。
class LiveMonitorWidget : public QWidget {
    Q_OBJECT
public:
    explicit LiveMonitorWidget(QWidget* parent = nullptr);
    ~LiveMonitorWidget() override;
    bool isPaused() const
    {
        return _paused;
    }
    void showInspection(const QJsonObject& result, const cv::Mat& image, bool toolTest = false);
    void setTriggerSettings(bool enabled, int intervalMs, bool modbus = false);

Q_SIGNALS:
    void editSchemeRequested();
    void newSchemeRequested();
    void importSchemeRequested();
    void exportSchemeRequested();
    void deleteSchemeRequested();
    void schemeSelectionRequested(const QString& schemeName);
    void runOnceRequested();
    void runToolRequested(const QString& toolId);
    void pausedChanged(bool paused);

public Q_SLOTS:
    void setSchemes(const QStringList& schemes, const QString& currentScheme);
    void setCurrentSchemeTools(const QList<SchemeTool>& tools);

protected:
    void showEvent(QShowEvent* event) override;
    void hideEvent(QHideEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private Q_SLOTS:
    void onFrameReady(const cv::Mat& frame);
    void onStreamError(const QString& message);
    void onFolderImageReady(const cv::Mat& image, const QString& fileName);
    void switchToCameraSource();
    void switchToFolderSource();
    void toggleRun();

private:
    void startStream();
    void stopStream();
    void refreshVisualization();
    void updateSystemUsage();

    ImageCanvasWidget* _canvas { nullptr };
    QLabel* _streamStatus { nullptr };
    QLabel* _resultStatus { nullptr };
    QLabel* _schemeTitle { nullptr };
    QListWidget* _schemeList { nullptr };
    QPushButton* _exportSchemeButton { nullptr };
    QPushButton* _editSchemeButton { nullptr };
    QPushButton* _listEditSchemeButton { nullptr };
    QTableWidget* _toolResultTable { nullptr };
    ImageInputPanel* _inputPanel { nullptr };
    cv::Mat _currentFrame;
    QString _currentSource { "camera" };
    bool _cameraLeaseHeld { false };
    bool _paused { false };
    quint64 _total { 0 }, _ok { 0 }, _ng { 0 }, _errors { 0 };
    QString _statsScheme;
    QWidget* _historyPanel { nullptr };
    QComboBox* _toolSelector { nullptr };
    QCheckBox* _liveView { nullptr };
    QPushButton* _testToolButton { nullptr };
    QPushButton* _pauseButton { nullptr };
    QLabel* _frameInfo { nullptr };
    QLabel* _toolDetail { nullptr };
    QLabel* _resultBadge { nullptr };
    QLabel* _triggerStatus { nullptr };
    QLabel* _systemUsageLabel { nullptr };
    QTimer* _systemUsageTimer { nullptr };
    SystemResourceMonitor _systemResources;
    InspectionTrendWidget* _trend { nullptr };
    QJsonObject _lastResult;
    cv::Mat _resultFrame;
};

#endif // LIVEMONITORWIDGET_H
