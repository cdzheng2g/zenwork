#ifndef CAMERASTREAMSERVICE_H
#define CAMERASTREAMSERVICE_H

#include "infrastructure/camera/CameraStreamLease.h"
#include "infrastructure/camera/LatestFrameMailbox.h"

#include <opencv2/core.hpp>

#include <QObject>
#include <QString>

class CameraStreamWorker;
class QTimer;
class CameraRecovery;

// 进程内唯一的采集服务。页面通过租约共享板端 VI->VPSS 帧流。
class CameraStreamService : public QObject {
    Q_OBJECT
public:
    static CameraStreamService& instance();

    void acquire();
    void release();
    // 主工作台运行期间保持一个常驻租约，页面切换只增减页面租约，不能导致
    // 驱动被重新打开并再次设置格式。
    void retainForApplicationLifetime();
    // 在停止AIQ前同步结束取帧线程，避免释放媒体资源时仍有VPSS取帧请求。
    void shutdown();
    void retry();

Q_SIGNALS:
    void frameReady(const cv::Mat& frame);
    void frameAvailable(const FrameEnvelope& frame);
    void cameraStateChanged(bool ready);
    void streamError(const QString& message);

private Q_SLOTS:
    void stopWhenUnused();

private:
    CameraStreamService();

    LatestFrameMailbox _frames;
    CameraStreamLease _lease;
    CameraStreamWorker* _worker { nullptr };
    QTimer* _stopTimer { nullptr };
    CameraRecovery* _recovery { nullptr };
    QString _lastError;
    bool _applicationLeaseHeld { false };
};

#endif // CAMERASTREAMSERVICE_H
