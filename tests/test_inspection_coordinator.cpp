#include "test_support.h"
#include "application/inspection/InspectionCoordinator.h"
#include "application/operators/OperatorRegistry.h"
#include <QSemaphore>
int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);
    Checks check;
    int applies = 0, stops = 0, outputs = 0;
    bool cameraOk = true, communicationOk = true;
    QSemaphore release;
    InspectionService service([&](const InspectionRequest&) {
        release.acquire(); return QJsonObject {{"status", "OK"}, {"elapsedMs", 3}};
    }, [&](const QStringList&, const QJsonObject&) { ++outputs; return QStringList {}; });
    InspectionCoordinator coordinator(service, {
        [&](const QJsonObject&, QString*) { ++applies; return cameraOk; },
        [&](const SchemeRuntimeSettings&, QString*) { return communicationOk; },
        [&] { ++stops; }
    });
    InspectionRequest request;
    request.scheme = "scheme-A";
    request.tools = {OperatorRegistry::makeTool("brightness")};
    request.settings.internalTriggerEnabled = false;
    request.settings.outputChannels.clear();
    QString error;
    check(coordinator.activate(request, &error) && applies == 1, "activation applies a validated snapshot through injected backends");
    cv::Mat image(4, 4, CV_8UC1, cv::Scalar(90));
    service.updateFrame(FrameEnvelope {image, 71, monotonicNowNs() - 2000000000LL});
    check(coordinator.submit(TriggerSource::Manual) == TriggerDecision::NotReady,
        "frame freshness uses acquisition time even if it just reached the GUI");
    service.updateFrame(FrameEnvelope {image, 72, monotonicNowNs() + 1000000000LL});
    check(!service.hasFreshFrame(), "future or invalid capture clocks do not pass freshness checks");
    service.updateFrame(FrameEnvelope {image, 73, monotonicNowNs()});
    coordinator.setEditing(true);
    check(coordinator.submit(TriggerSource::Manual) == TriggerDecision::NotReady, "editing blocks production without querying QWidget state");
    coordinator.setEditing(false);
    coordinator.setFileBusy(true);
    check(coordinator.submit(TriggerSource::Manual) == TriggerDecision::NotReady, "explicit file transaction state blocks production");
    coordinator.setFileBusy(false);
    coordinator.setPaused(true);
    check(coordinator.submit(TriggerSource::Internal) == TriggerDecision::NotReady,
        "paused mode blocks automatic triggers");
    check(coordinator.submit(TriggerSource::Manual) == TriggerDecision::Accepted
        && coordinator.submit(TriggerSource::Manual) == TriggerDecision::Busy,
        "manual inspection while paused remains allowed and overlapping work stays blocked");
    request.scheme = "scheme-B";
    coordinator.activate(request);
    release.release();
    check(until([&] { return !service.isRunning(); }) && service.discarded() && outputs == 0,
        "activating another scheme invalidates the in-flight old result");
    coordinator.setPaused(false);
    service.updateFrame(FrameEnvelope {image, 74, monotonicNowNs()});
    coordinator.submit(TriggerSource::Manual); release.release();
    check(until([&] { return !service.isRunning(); }) && outputs == 1
        && service.result()["trace"].toObject()["frameId"] == "74"
        && service.result()["timings"].toObject()["completionMs"].toDouble() >= 0,
        "headless coordinator produces one identified result with frame and stage timings");
    cameraOk = false;
    check(!coordinator.activate(request, &error) && !error.isEmpty() && stops == 1,
        "partial activation fails closed and stops communication");
    service.updateFrame(image);
    check(coordinator.submit(TriggerSource::Manual) == TriggerDecision::NotReady,
        "failed activation cannot run a stale scheme even with a fresh frame");
    cameraOk = true; communicationOk = false;
    check(!coordinator.activate(request, &error) && stops == 2, "communication activation failure also blocks execution");
    communicationOk = true;
    check(coordinator.activate(request), "corrected settings can activate normally");
    coordinator.deactivate();
    service.updateFrame(image);
    check(coordinator.submit(TriggerSource::Manual) == TriggerDecision::NotReady, "deactivation clears business readiness");
    coordinator.shutdown();
    return check.result();
}
