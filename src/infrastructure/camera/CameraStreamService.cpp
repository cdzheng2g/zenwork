#include "infrastructure/camera/CameraStreamService.h"

#include "infrastructure/camera/CameraStreamWorker.h"
#include "infrastructure/camera/CameraController.h"

#include <QTimer>

namespace {
constexpr int kStopGracePeriodMs = 1000;
constexpr const char* kCameraDevice = "/dev/video11";
constexpr int kCameraWidth = 1280;
constexpr int kCameraHeight = 1024;
} // namespace

CameraStreamService& CameraStreamService::instance()
{
    static CameraStreamService service;
    return service;
}

CameraStreamService::CameraStreamService()
{
    _worker = new CameraStreamWorker(this);
    _worker->setDevice(QString::fromUtf8(kCameraDevice));
    _worker->setResolution(kCameraWidth, kCameraHeight);
    connect(
        _worker, &CameraStreamWorker::frameReady, this,
        [this](const cv::Mat& frame) {
            if (!_frames.offer(frame))
                return;
            QMetaObject::invokeMethod(
                this,
                [this] {
                    const auto latest = _frames.take();
                    if (!latest.empty())
                        Q_EMIT frameReady(latest);
                },
                Qt::QueuedConnection);
        },
        Qt::DirectConnection);
    connect(_worker, &CameraStreamWorker::streamError, this, &CameraStreamService::streamError);

    _stopTimer = new QTimer(this);
    _stopTimer->setSingleShot(true);
    connect(_stopTimer, &QTimer::timeout, this, &CameraStreamService::stopWhenUnused);
}

void CameraStreamService::acquire()
{
    const bool shouldStart = _lease.acquire();
    _stopTimer->stop();
    if (shouldStart) {
#ifdef HAVE_ROCKIT_MPI
        // Rockit的AIQ、VI、VPSS初始化必须保持在调用initialize()的Qt主线程。
        // 参考cam_ctl也是在主线程按此顺序完成，工作线程仅GetChnFrame。
        if (!CameraController::instance().startQtFrameStream(kCameraWidth, kCameraHeight)) {
            _lease.release();
            Q_EMIT streamError(QStringLiteral("启动AIQ/VI/VPSS预览链路失败"));
            return;
        }
#endif
        _worker->startCapture();
    }
}

void CameraStreamService::release()
{
    if (_lease.release())
        _stopTimer->start(kStopGracePeriodMs);
}

void CameraStreamService::retainForApplicationLifetime()
{
    if (_applicationLeaseHeld)
        return;
    _applicationLeaseHeld = true;
    acquire();
}

void CameraStreamService::shutdown()
{
    _stopTimer->stop();
    _lease = CameraStreamLease { };
    _applicationLeaseHeld = false;
    _worker->stop();
    CameraController::instance().stopQtFrameStream();
}

void CameraStreamService::stopWhenUnused()
{
    if (_lease.consumerCount() == 0) {
        _worker->stop();
        CameraController::instance().stopQtFrameStream();
    }
}
