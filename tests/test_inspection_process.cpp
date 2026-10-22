#include "test_support.h"
#include "infrastructure/inference/InspectionProcess.h"
#include "application/inspection/InspectionWireCodec.h"
#include "application/inspection/InspectionService.h"
#include <thread>
#include <QJsonDocument>
#include <QTemporaryDir>
#include <QFile>
#include <QFileInfo>

int main(int argc, char** argv) {
    if (argc > 1 && QByteArray(argv[1]) == "--inspection-worker")
        return inspectionWorkerMain(argc, argv, [](const InspectionRequest& r) {
            if (r.scheme == "hang") {
                QFile entered(r.reference.imagePath);
                if (!entered.open(QIODevice::WriteOnly)) return QJsonObject {{"status", "ERROR"}};
                entered.write("entered"); entered.close();
                for (;;) QThread::msleep(100);
            }
            static int calls = 0;
            return QJsonObject {{"status", "OK"}, {"pid", double(QCoreApplication::applicationPid())},
                {"calls", ++calls}, {"pixel", r.image.at<uchar>(0,0)},
                {"referencePixel", r.reference.image.at<uchar>(0,0)},
                {"traceEcho", r.trace.toJson()}, {"path", r.reference.imagePath}};
        });
    QCoreApplication app(argc, argv);
    Checks check;
    InspectionRequest r;
    r.scheme = "normal";
    r.image = cv::Mat(8, 9, CV_8UC1, cv::Scalar(42));
    r.reference.image = cv::Mat(3, 4, CV_8UC1, cv::Scalar(81));
    r.reference.imagePath = "/not-read/reference.png";
    r.trace = {"one", 9007199254740993ULL, 4, monotonicNowNs(), monotonicNowNs(), 0};
    InspectionProcess process({}, 1000);
    auto a = process.execute(r);
    r.trace.inspectionId = "two";
    auto b = process.execute(r);
    if (a["status"] != "OK") std::fprintf(stderr, "first response: %s\n", QJsonDocument(a).toJson(QJsonDocument::Compact).constData());
    if (b["status"] != "OK") std::fprintf(stderr, "second response: %s\n", QJsonDocument(b).toJson(QJsonDocument::Compact).constData());
    check(a["status"] == "OK" && a["pixel"] == 42 && a["referencePixel"] == 81
        && a["path"] == r.reference.imagePath,
        "real subprocess transports lossless image and reference snapshots");
    check(a["pid"] == b["pid"] && b["calls"] == 2 && a["pid"].toDouble() != QCoreApplication::applicationPid(),
        "persistent worker is a separate process and reuses its execution context");
    check(a["traceEcho"].toObject()["frameId"] == "9007199254740993", "IPC preserves 64-bit frame identity");
    QTemporaryDir directory;
    r.reference.imagePath = directory.filePath("hung-entered");
    r.scheme = "hang"; r.trace.inspectionId = "hung";
    QElapsedTimer elapsed; elapsed.start();
    auto hung = process.execute(r);
    check(QFileInfo::exists(r.reference.imagePath) && hung["recoveryRequired"].toBool()
        && hung["status"] == "ERROR" && elapsed.elapsed() < 2500,
        "unresponsive child is killed and yields a bounded recovery fault");
    r.cancelled = std::make_shared<std::atomic_bool>(false);
    r.reference.imagePath = directory.filePath("cancel-entered");
    std::thread cancel([flag=r.cancelled, marker=r.reference.imagePath] {
        QElapsedTimer age; age.start();
        while (!QFileInfo::exists(marker) && age.elapsed() < 2000) QThread::msleep(5);
        flag->store(true);
    });
    elapsed.restart();
    auto cancelled = process.execute(r);
    cancel.join();
    check(QFileInfo::exists(r.reference.imagePath) && cancelled["recoveryRequired"].toBool() && elapsed.elapsed() < 2500,
        "shutdown cancellation interrupts IPC even when the SDK never returns");
    {
        InspectionService service([](const InspectionRequest& request) {
            thread_local InspectionProcess worker({}, 10000);
            return worker.execute(request);
        }, {});
        service.updateFrame(r.image);
        InspectionGate gate {true,true,false,false,false,false,false,true};
        service.start(r, TriggerSource::Manual, gate);
        QThread::msleep(30);
        elapsed.restart(); service.shutdown();
        check(elapsed.elapsed() < 2500, "InspectionService shutdown cancels and joins its isolated executor");
        check(!service.isRunning() && service.start(r, TriggerSource::Manual, gate) == TriggerDecision::NotReady,
            "shutdown closes admission and clears busy without publishing a queued late result");
    }
    return check.result();
}
