#pragma once
#include "application/operators/OperatorExecution.h"
#include "contracts/ImageInputSpec.h"
#include <QRectF>
#include <optional>

struct OperatorImageInput {
    cv::Mat image;
    QRectF sourceRect;
    cv::Mat toImage;
    QJsonArray polygon(const QRectF& localRect) const;
};

QString validateImageInput(const QJsonObject& parameters, const ImageInputSpec& spec);
// Alignment failure sets SKIPPED and returns no image; invalid ROI throws an error.
std::optional<OperatorImageInput> prepareOperatorImage(const OperatorRunContext& context,
    const QJsonObject& parameters, OperatorOutcome& outcome, const ImageInputSpec& spec = {});
