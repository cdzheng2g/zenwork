#ifndef CAMERASTREAMWORKER_H
#define CAMERASTREAMWORKER_H

#include <opencv2/core.hpp>

#include <QThread>
#include <QString>

#include <atomic>

// 后台取帧线程。RK3576 板端从 VI->VPSS 读取；非 Rockit 构建保留
// V4L2 开发机回退路径。帧交给 CameraStreamService 限流后投递到 UI。
class CameraStreamWorker : public QThread {
    Q_OBJECT
public:
    explicit CameraStreamWorker(QObject* parent = nullptr);
    ~CameraStreamWorker() override;

    // 非 Rockit 回退路径的设备节点，须在 startCapture() 前设置。
    void setDevice(const QString& devicePath);
    // 期望的采集分辨率;传0表示用设备默认。须在start()前调用。
    void setResolution(int width, int height);

    // 在启动线程前设置采集请求，避免 stop() 与 run() 初始化阶段竞争。
    void startCapture();

    // 请求停止取流并等待线程退出(析构时也会自动调用)。
    void stop();

Q_SIGNALS:
    // 成功取到一帧(BGR 格式 cv::Mat)。
    void frameReady(const cv::Mat& frame);
    // 打开设备失败或取流中断,message为可读的原因,供UI提示。
    void streamError(const QString& message);

protected:
    void run() override;

private:
    QString _devicePath;
    int _width { 0 };
    int _height { 0 };
    std::atomic<bool> _running { false };
    std::atomic<int> _wakeFd { -1 };
};

#endif // CAMERASTREAMWORKER_H
