#include "ui/FileOperation.h"
#include "application/scheme/SchemePackage.h"
#include "infrastructure/reference/ReferenceImageManager.h"
#include "infrastructure/camera/LatestFrameMailbox.h"
#include <QTemporaryDir>
#include <QFile>
#include <QElapsedTimer>
#include <cstdio>
#include <memory>

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    QTemporaryDir temp;
    qputenv("XDG_DATA_HOME", temp.path().toUtf8());
    int failures = 0;
    auto check = [&](bool ok, const char* name) {
        std::printf("[%s] %s\n", ok ? "PASS" : "FAIL", name);
        std::fflush(stdout);
        if (!ok)
            ++failures;
    };
    LatestFrameMailbox mailbox;
    int deliveries = 0;
    for (int i = 0; i < 200; ++i) {
        cv::Mat frame(10, 10, CV_32SC1, cv::Scalar(i));
        if (mailbox.offer(frame))
            ++deliveries;
    }
    check(deliveries == 1, "blocked UI queues only one frame delivery");
    check(mailbox.take().at<int>(0, 0) == 199, "blocked UI keeps latest frame, discards stale frames");
    check(mailbox.offer(cv::Mat(10, 10, CV_32SC1, cv::Scalar(200))),
        "frame delivery resumes after draining mailbox");
    if (argc < 2)
        return failures ? 1 : 0;
    QWidget host;
    host.resize(640, 400);
    host.show();
    QElapsedTimer elapsed;
    elapsed.start();
    qint64 last = 0, maxGap = 0;
    int ticks = 0;
    QTimer heartbeat;
    heartbeat.setInterval(20);
    QObject::connect(&heartbeat, &QTimer::timeout, &host, [&] {
        auto now = elapsed.elapsed();
        maxGap = qMax(maxGap, now - last);
        last = now;
        ++ticks;
    });
    heartbeat.start();
    const QString source = QString::fromLocal8Bit(argv[1]);
    auto package = std::make_shared<SchemePackage>();
    FileOperation::run(
        &host, "Real YAML round trip",
        [&, package, source] {
            QString error;
            if (!SchemePackage::load(source, *package, &error))
                return error;
            SchemeRuntimeSettings config;
            if (!package->installResources(temp.path() + "/resources", config, &error))
                return error;
            package->reference.modelName = "async-test";
            if (!ReferenceImageManager::savePreparedReference(package->reference, &error))
                return error;
            if (!package->save(temp.path() + "/export.yaml", &error))
                return error;
            return QString();
        },
        [&, package](const QString& error) {
            check(error.isEmpty(), "actual image/model YAML import and export complete in worker");
            if (!error.isEmpty())
                std::fprintf(stderr, "%s\n", error.toUtf8().constData());
            check(ticks > 10 && maxGap < 1000, "GUI heartbeat continues throughout large file operation");
            check(!FileOperation::busy(&host), "busy state clears after success");
            ReferenceImageManager::instance().adoptPreparedReference(package->reference);
            check(ReferenceImageManager::instance().contains("async-test"),
                "completed reference published on GUI thread");
            std::printf("ticks=%d maxGapMs=%lld elapsedMs=%lld\n", ticks, (long long)maxGap,
                (long long)elapsed.elapsed());
            FileOperation::run(
                &host, "Failure recovery",
                []() -> QString { throw std::runtime_error("injected read failure"); },
                [&](const QString& failure) {
                    check(failure.contains("injected"), "worker exception becomes visible error");
                    check(!FileOperation::busy(&host), "busy state clears after failure");
                    app.exit(failures ? 1 : 0);
                });
        });
    check(!FileOperation::run(
              &host, "Duplicate", [] { return QString(); }, [](const QString&) { }),
        "duplicate file operations rejected while busy");
    return app.exec();
}
