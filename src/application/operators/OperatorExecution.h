#pragma once

#include "application/InspectionRunner.h"
#include "domain/project/LogicEvaluator.h"

#include <QJsonObject>
#include <opencv2/core.hpp>

struct OperatorRunContext {
    explicit OperatorRunContext(const InspectionRequest& inspection)
        : request(inspection)
        , aligned(inspection.image)
    {
    }

    const InspectionRequest& request;
    cv::Mat aligned;
    cv::Mat alignedToImage;
    bool alignmentFailed { false };
};

struct OperatorOutcome {
    NodeState state { NodeState::Error };
    QString error;
    QJsonObject input;
    QJsonObject fields;
};

using OperatorExecutor = void (*)(OperatorRunContext&, const QJsonObject&, OperatorOutcome&);
