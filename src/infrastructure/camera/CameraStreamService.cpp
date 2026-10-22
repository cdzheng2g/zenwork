#include "infrastructure/camera/CameraStreamService.h"

#include "infrastructure/camera/CameraStreamWorker.h"
#include "infrastructure/camera/CameraController.h"
#include "infrastructure/camera/CameraRecovery.h"

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
    _recovery = new CameraRecovery(
        [this] {
            if (_worker->isRunning()) return false;
            _lastError.clear();
#ifdef HAVE_ROCKIT_MPI
            auto& camera = CameraController::instance();
            camera.stopQtFrameStream();
            if (!camera.isInitialized() && !camera.initialize()) return false;
            if (!camera.startQtFrameStream(kCameraWidth, kCameraHeight)) return false;
#endif
            _worker->startCapture();
            return true;
        },
        [this] {
            _worker->stop();
            CameraController::instance().stopQtFrameStream();
        }, this);
    connect(_recovery, &CameraRecovery::stateChanged, this, [this](bool ready) {
        if (!ready) (void)_frames.takeFrame();
        emit cameraStateChanged(ready);
    });
    connect(_recovery, &CameraRecovery::errorOccurred, this, &CameraStreamService::streamError);
    connect(
        _worker, &CameraStreamWorker::frameReady, this,
        [this](const FrameEnvelope& frame) {
            if (!_frames.offerFrame(frame))
                return;
            QMetaObject::invokeMethod(
                this,
                [this] {
                    const auto latest = _frames.takeFrame();
                    if (!latest.image.empty()) {
                        _recovery->frameReceived();
                        Q_EMIT frameAvailable(latest);
                        Q_EMIT frameReady(latest.image);
                    }
                },
                Qt::QueuedConnection);
        },
        Qt::DirectConnection);
    connect(_worker, &CameraStreamWorker::streamError, this, [this](const QString& reason) {
        _lastError = reason;
        emit streamError(reason);
        emit cameraStateChanged(false);
    });
    connect(_worker, &QThread::finished, this, [this] {
        // A queued finished signal can belong to a stream already stopped and restarted.
        if (!_worker->isRunning()) _recovery->workerStopped(_lastError);
    });

    _stopTimer = new QTimer(this);
    _stopTimer->setSingleShot(true);
    connect(_stopTimer, &QTimer::timeout, this, &CameraStreamService::stopWhenUnused);
}

void CameraStreamService::acquire()
{
    const bool shouldStart = _lease.acquire();
    _stopTimer->stop();
    // Demand survives a failed start; recovery state is independent of lease count.
    if (shouldStart) _recovery->setDemand(true);
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
    _recovery->setDemand(false);
}

void CameraStreamService::stopWhenUnused()
{
    if (_lease.consumerCount() == 0) {
        _recovery->setDemand(false);
    }
}
void CameraStreamService::retry()
{
    _recovery->retry();
}
