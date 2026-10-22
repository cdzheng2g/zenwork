#pragma once
#include "contracts/SchemeConfiguration.h"
#include "contracts/ReferenceImage.h"
#include <QJsonObject>
#include "contracts/InspectionTrace.h"
#include <atomic>
#include <memory>

// Value-only snapshot: the worker never accesses widgets or mutable singletons.
struct InspectionRequest {
    QString scheme;
    SchemeRuntimeSettings settings;
    QList<SchemeTool> tools;
    cv::Mat image;
    ReferenceImage reference;
    InspectionTrace trace;
    std::shared_ptr<std::atomic_bool> cancelled;
};
QJsonObject runInspection(const InspectionRequest& request);
