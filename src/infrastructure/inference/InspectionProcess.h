#pragma once
#include "application/InspectionRunner.h"
#include <functional>
#include <memory>

// One persistent child per dedicated parent execution thread. No GUI objects cross IPC.
class InspectionProcess final {
public:
    explicit InspectionProcess(QString program = {}, int timeoutMs = 60000);
    ~InspectionProcess();
    QJsonObject execute(const InspectionRequest& request);
private:
    struct Impl;
    std::unique_ptr<Impl> _impl;
};
QJsonObject runIsolatedInspection(const InspectionRequest& request);
int inspectionWorkerMain(int argc, char** argv,
    std::function<QJsonObject(const InspectionRequest&)> executor = &runInspection);
