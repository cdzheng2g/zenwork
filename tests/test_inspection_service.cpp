#include "application/inspection/InspectionService.h"
#include <QCoreApplication>
#include <QEventLoop>
#include <QTimer>
#include <QThread>
#include <cstdio>
int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);
    int failures = 0, deliveries = 0;
    auto check = [&](bool ok, const char* text) {
        printf("[%s] %s\n", ok ? "PASS" : "FAIL", text);
        if (!ok)
            ++failures;
    };
    InspectionService service(
        [](const InspectionRequest& request) {
            QThread::msleep(30);
            return QJsonObject { { "scheme", request.scheme }, { "status", "OK" },
                { "pixel", request.image.at<uchar>(0, 0) } };
        },
        [&](const QStringList&, const QJsonObject&) {
            ++deliveries;
            return QStringList { };
        });
    auto wait = [&] {
        QEventLoop loop;
        QTimer timeout;
        timeout.setSingleShot(true);
        QObject::connect(&service, &InspectionService::finished, &loop, &QEventLoop::quit);
        QObject::connect(&timeout, &QTimer::timeout, &loop, &QEventLoop::quit);
        timeout.start(3000);
        loop.exec();
        check(!service.isRunning(), "background execution completes");
    };
    InspectionRequest request;
    request.scheme = "test";
    request.tools = { { "test-tool", "Template", "template" } };
    InspectionGate gate { true, true, false, false, false, true, false, true };
    check(service.start(request, TriggerSource::Manual, gate) == TriggerDecision::NotReady,
        "no frame rejects trigger");
    cv::Mat frame(5, 5, CV_8UC1, cv::Scalar(10));
    service.updateFrame(frame);
    check(service.start(request, TriggerSource::Manual, gate) == TriggerDecision::Accepted,
        "first trigger accepted");
    check(service.start(request, TriggerSource::Manual, gate) == TriggerDecision::Busy,
        "overlapping request rejected");
    frame.setTo(99);
    service.updateFrame(frame);
    wait();
    check(service.result()["pixel"].toInt() == 10 && deliveries == 1,
        "worker owns frame snapshot and delivers once");
    service.start(request, TriggerSource::Manual, gate);
    service.invalidate();
    wait();
    check(service.discarded() && deliveries == 1, "switching scheme discards old in-flight result");
    service.start(request, TriggerSource::Manual, gate, "test-tool");
    wait();
    check(service.isToolTest() && deliveries == 1, "single-tool test never sends production output");
    service.start(request, TriggerSource::Manual, gate);
    wait();
    check(service.result()["sequence"].toInt() == 2 && deliveries == 2,
        "discarded and test results do not advance production sequence");
    return failures ? 1 : 0;
}
