#include "test_support.h"
#include "infrastructure/camera/CameraRecovery.h"
#include "infrastructure/persistence/ArchiveQueue.h"
#include "application/communication/ResultDeliveryTracker.h"
#include <QSemaphore>
#include <QTimer>
#include <atomic>

int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);
    Checks check;
    int starts = 0, stops = 0;
    bool canStart = false;
    CameraRecovery camera([&] { ++starts; return canStart; }, [&] { ++stops; }, nullptr, 2, 2);
    camera.setDemand(true);
    check(until([&] { return camera.state() == CameraRecovery::State::Failed; }) && starts == 3,
        "initial camera start plus two retries stop at a finite fault");
    canStart = true;
    camera.retry();
    camera.frameReceived();
    check(starts == 4 && camera.state() == CameraRecovery::State::Running,
        "manual recovery works while consumer demand remains present");
    camera.workerStopped("lost stream");
    check(until([&] { return starts == 5; }), "unexpected worker exit restarts without a new lease");
    camera.frameReceived();
    camera.workerStopped("lost again");
    camera.setDemand(false);
    QElapsedTimer wait; wait.start();
    until([&] { return wait.elapsed() > 20; });
    check(starts == 5 && stops == 1 && camera.state() == CameraRecovery::State::Stopped,
        "removing demand cancels pending retries");

    QSemaphore entered, release;
    std::atomic<int> writes {0};
    QList<DeliveryEvent> receipts;
    ArchiveQueue archive([&](const QByteArray&) {
        ++writes; entered.release(); release.acquire(); return QString();
    }, nullptr, 2, 2048);
    QObject::connect(&archive, &ArchiveQueue::completed, &archive,
        [&](const DeliveryEvent& event) { receipts << event; });
    QString error;
    check(archive.enqueue({{"inspectionId", "first"}, {"sequenceText", "9007199254740993"}}, &error),
        "archive accepts first record");
    check(entered.tryAcquire(1, 1000), "writer starts in its own thread");
    check(archive.enqueue({{"inspectionId", "second"}}, &error)
        && !archive.enqueue({{"inspectionId", "third"}}, &error) && !error.isEmpty(),
        "archive count limit includes the in-flight record and rejects explicitly");
    bool heartbeat = false;
    QTimer::singleShot(1, &archive, [&] { heartbeat = true; });
    check(until([&] { return heartbeat; }, 100), "GUI event loop remains responsive while storage is blocked");
    release.release(2);
    check(until([&] { return receipts.size() == 2; }), "archive returns completion for each queued item");
    check(receipts.size() == 2 && receipts[0].inspectionId == "first"
        && receipts[0].sequenceText == "9007199254740993"
        && receipts[1].inspectionId == "second" && writes == 2,
        "archive preserves order and full precision identity across threads");
    check(archive.shutdown() && !archive.enqueue({}, &error), "drained archive closes admission");
    ArchiveQueue failing([](const QByteArray&) { return QString("disk full"); }, nullptr, 4, 100);
    DeliveryEvent failed;
    QObject::connect(&failing, &ArchiveQueue::completed, &failing,
        [&](const DeliveryEvent& e) { failed = e; });
    check(!failing.enqueue({{"payload", QString(200, 'x')}}, &error), "archive enforces serialized byte budget");
    failing.enqueue({{"inspectionId", "disk"}}, &error);
    check(until([&] { return failed.inspectionId == "disk"; }) && failed.state == DeliveryState::Failed
        && failed.error == "disk full", "filesystem failure is correlated with the originating record");
    failing.shutdown();

    ResultDeliveryTracker tracker(nullptr, 15, 2);
    const auto result = [](const char* id) { return QJsonObject {{"inspectionId", id}, {"sequenceText", id}}; };
    check(tracker.begin("modbus", result("A")) && tracker.begin("modbus", result("B"))
        && !tracker.begin("modbus", result("C"), &error), "delivery tracker bounds outstanding results");
    tracker.finish({"B", "B", "modbus", DeliveryState::Failed, "wire error"});
    tracker.finish({"A", "A", "modbus", DeliveryState::Confirmed, {}});
    check(tracker.latest("A", "modbus").state == DeliveryState::Confirmed
        && tracker.latest("B", "modbus").state == DeliveryState::Failed,
        "out-of-order responses stay associated with their inspection IDs");
    check(tracker.begin("modbus", result("C")), "terminal history is evicted to accept new delivery");
    check(until([&] { return tracker.latest("C", "modbus").state == DeliveryState::TimedOut; }),
        "pending delivery expires without a response");
    tracker.finish({"C", "C", "modbus", DeliveryState::Confirmed, {}});
    check(tracker.latest("C", "modbus").state == DeliveryState::TimedOut,
        "late ACK cannot silently rewrite a terminal timeout");
    return check.result();
}
