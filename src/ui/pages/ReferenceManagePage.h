#ifndef REFERENCEMANAGEPAGE_H
#define REFERENCEMANAGEPAGE_H

#include <QWidget>

#include <opencv2/core.hpp>

class ElaComboBox;
class ElaPushButton;
class ElaText;
class ElaLineEdit;
class ImageCanvasWidget;

// 基准图管理页:导入原始基准图、按产品型号切换基准图集、标记基准区域
// (referenceRect,即模板匹配的模板来源)。
class ReferenceManagePage : public QWidget {
    Q_OBJECT
public:
    explicit ReferenceManagePage(QWidget* parent = nullptr);
    void selectModel(const QString& name);

Q_SIGNALS:
    // 基准图集发生变化(导入/切换型号),供其他页面(如校正执行页)刷新型号列表
    void referenceModelsChanged();

private Q_SLOTS:
    void onImportClicked();
    void onCaptureLiveFrameClicked();
    void onModelChanged(const QString& modelName);
    void onMarkRectToggled(bool checked);
    void onCanvasPointMarked(const QPointF& imagePoint);
    void onLiveFrameReady(const cv::Mat& frame);

protected:
    void showEvent(QShowEvent* event) override;
    void hideEvent(QHideEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    QWidget* _filePanel { nullptr };
    void startReferenceImport(const QString& model, const QString& path, const cv::Mat& frame = { });
    void initContent();
    void refreshModelList();
    void refreshCurrentModelView();

    ElaComboBox* _modelComboBox { nullptr };
    ElaLineEdit* _modelNameEdit { nullptr };
    ElaPushButton* _importButton { nullptr };
    ElaPushButton* _captureLiveButton { nullptr };
    ElaPushButton* _markRectButton { nullptr };
    ElaText* _statusText { nullptr };
    ImageCanvasWidget* _canvas { nullptr };

    bool _markingRectFirstCorner { false };
    QPointF _rectFirstCorner;
    cv::Mat _latestLiveFrame;
    bool _cameraLeaseHeld { false };
};

#endif // REFERENCEMANAGEPAGE_H
