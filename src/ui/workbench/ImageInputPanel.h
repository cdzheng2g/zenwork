#ifndef IMAGEINPUTPANEL_H
#define IMAGEINPUTPANEL_H

#include "infrastructure/input/ImageSequenceSource.h"

#include <opencv2/core.hpp>

#include <QWidget>

class ElaComboBox;
class ElaPushButton;
class ElaText;
class QTimer;

// 运行管理的图像输入选择器:实时相机或文件夹图片序列。目录回放用于现场
// 没有接入相机时从头测试方案、基准图和算子配置。
class ImageInputPanel : public QWidget {
    Q_OBJECT
public:
    explicit ImageInputPanel(QWidget* parent = nullptr);
    ~ImageInputPanel() override;

    bool isCameraSource() const;

Q_SIGNALS:
    void cameraSourceSelected();
    void folderSourceSelected();
    void imageReady(const cv::Mat& image, const QString& fileName);
    void sourceStatusChanged(const QString& status);

private Q_SLOTS:
    void onSourceChanged(int index);
    void selectDirectory();
    void showPrevious();
    void showNext();
    void togglePlayback();
    void playbackTick();

private:
    void initContent();
    void updateControls();
    void loadCurrentImage();
    void setStatus(const QString& status, bool error = false);

    ElaComboBox* _sourceComboBox { nullptr };
    ElaPushButton* _directoryButton { nullptr };
    ElaPushButton* _previousButton { nullptr };
    ElaPushButton* _nextButton { nullptr };
    ElaPushButton* _playButton { nullptr };
    ElaText* _directoryText { nullptr };
    ElaText* _fileText { nullptr };
    ElaText* _statusText { nullptr };
    QTimer* _playbackTimer { nullptr };
    ImageSequenceSource _sequence;
};

#endif // IMAGEINPUTPANEL_H
