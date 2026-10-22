#include "application/InspectionRunner.h"
#include "application/operators/OperatorRegistry.h"
#include "infrastructure/inference/rknn/RknnSession.h"
#include <QCoreApplication>
#include <cstdio>

namespace {
bool sdkTimeout = true;
int followingCalls = 0;
void fault(OperatorRunContext&, const QJsonObject&, OperatorOutcome&)
{
    if (sdkTimeout) throw RknnTimeout("fake SDK timeout");
    throw std::runtime_error("ordinary operator error");
}
void following(OperatorRunContext&, const QJsonObject&, OperatorOutcome& out)
{
    ++followingCalls;
    out.state = NodeState::Ok;
}
}

// Link-time registry substitute: test the actual runner without hardware or models.
const QList<OperatorDefinition>& OperatorRegistry::definitions()
{
    static const auto definitions = [] {
        OperatorDefinition first, second;
        first.kind = "fault"; first.name = "fault"; first.execute = &fault;
        second.kind = "following"; second.name = "following"; second.execute = &following;
        return QList<OperatorDefinition> {first, second};
    }();
    return definitions;
}

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);
    int failures = 0;
    auto check = [&](bool ok, const char* label) {
        std::printf("[%s] %s\n", ok ? "PASS" : "FAIL", label);
        failures += !ok;
    };
    InspectionRequest request;
    request.image = cv::Mat(8, 8, CV_8UC3, cv::Scalar(0));
    request.tools = {OperatorRegistry::makeTool("fault"), OperatorRegistry::makeTool("following")};
    request.settings.logicOr = true;
    auto result = runInspection(request);
    check(result["status"] == "ERROR" && result["recoveryRequired"].toBool(),
        "SDK timeout survives the runner as a recovery fault, including OR schemes");
    check(followingCalls == 0 && result["tools"].toArray().size() == 1,
        "no following operator executes after an SDK timeout");
    sdkTimeout = false;
    result = runInspection(request);
    check(result["status"] == "ERROR" && !result["recoveryRequired"].toBool() && followingCalls == 1,
        "ordinary operator errors preserve existing continuation behavior without a recovery latch");
    return failures ? 1 : 0;
}
