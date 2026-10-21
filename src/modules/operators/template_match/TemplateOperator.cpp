#include "TemplateOperator.h"
#include "application/operators/OperatorGeometry.h"
#include "modules/operators/template_match/TemplateMatchCorrector.h"

#include <QJsonArray>
#include <opencv2/imgproc.hpp>
#include <stdexcept>

void executeTemplateOperator(
    OperatorRunContext& context, const QJsonObject& parameters, OperatorOutcome& outcome)
{
    const auto& request = context.request;
    if (request.reference.image.empty() || request.reference.referenceRect.isEmpty())
        throw std::runtime_error("请为方案选择基准图并标注区域");

    const auto roi = request.reference.referenceRect;
    outcome.input["roi"] = QJsonArray { roi.x(), roi.y(), roi.width(), roi.height() };
    outcome.input["referenceModel"] = request.settings.referenceModel;

    thread_local TemplateMatchCorrector corrector;
    const auto match = corrector.correct(request.reference.image, request.image, roi);
    outcome.state = match.success ? NodeState::Ok : NodeState::Ng;
    context.alignmentFailed = !match.success;
    outcome.fields = { { "score", match.matchScore }, { "dx", match.dx }, { "dy", match.dy },
        { "angle", match.angle }, { "message", match.message } };
    if (match.success) {
        context.aligned = match.correctedImage;
        cv::invertAffineTransform(match.affineMatrix, context.alignedToImage);
        outcome.fields["polygon"] = operatorImagePolygon(roi, context.alignedToImage);
    }
}
