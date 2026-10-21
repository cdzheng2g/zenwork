#include "OcrOperator.h"
#include "OcrEngine.h"
#include "application/vision/ImageInput.h"

#include <QJsonArray>
#include <QRectF>
#include <stdexcept>

void executeOcrOperator(OperatorRunContext& context, const QJsonObject& parameters, OperatorOutcome& outcome)
{
    const auto input = prepareOperatorImage(context, parameters, outcome);
    if (!input)
        return;
    outcome.fields = runOcrEngine(input->image, parameters);
    outcome.fields["polygon"] = input->polygon(QRectF(0, 0, input->image.cols, input->image.rows));
    QJsonArray mappedRegions;
    for (const auto& value : outcome.fields.value("regions").toArray()) {
        auto region = value.toObject();
        const QRectF rect(region.value("x").toDouble(),
            region.value("y").toDouble(), region.value("width").toDouble(),
            region.value("height").toDouble());
        region["polygon"] = input->polygon(rect);
        mappedRegions.append(region);
    }
    outcome.fields["regions"] = mappedRegions;

    outcome.state = NodeState::Ok;
}
