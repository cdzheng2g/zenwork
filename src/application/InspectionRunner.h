#pragma once
#include "contracts/SchemeConfiguration.h"
#include "contracts/ReferenceImage.h"
#include <QJsonObject>

// Value-only snapshot: the worker never accesses widgets or mutable singletons.
struct InspectionRequest {
    QString scheme;
    SchemeRuntimeSettings settings;
    QList<SchemeTool> tools;
    cv::Mat image;
    ReferenceImage reference;
};
QJsonObject runInspection(const InspectionRequest& request);
