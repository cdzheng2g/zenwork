#include "ImageInput.h"
#include "application/operators/OperatorGeometry.h"
#include <cmath>
#include <stdexcept>

namespace {
bool flag(const QJsonObject& parameters, const QString& key, bool fallback)
{
    return key.isEmpty() ? fallback : parameters.value(key).toBool(fallback);
}
}

QString validateImageInput(const QJsonObject& parameters, const ImageInputSpec& spec)
{
    for (const auto& key : {spec.fullImageKey, spec.alignedKey})
        if (!key.isEmpty() && parameters.contains(key) && !parameters[key].isBool())
            return "图像输入开关必须为布尔值：" + key;
    if (!spec.roiKey.isEmpty() && parameters.contains(spec.roiKey)) {
        if (!parameters[spec.roiKey].isArray())
            return "算子区域格式无效";
        const auto roi = parameters[spec.roiKey].toArray();
        if (roi.size() != 4)
            return "算子区域格式无效";
        for (const auto& value : roi)
            if (!value.isDouble() || !std::isfinite(value.toDouble()))
                return "算子区域必须为有限数值";
        if (roi[0].toDouble() < 0 || roi[1].toDouble() < 0 || roi[2].toDouble() <= 0
            || roi[3].toDouble() <= 0)
            return "算子区域无效";
    }
    return {};
}

QJsonArray OperatorImageInput::polygon(const QRectF& localRect) const
{
    return operatorImagePolygon(localRect, toImage);
}

std::optional<OperatorImageInput> prepareOperatorImage(const OperatorRunContext& context,
    const QJsonObject& parameters, OperatorOutcome& outcome, const ImageInputSpec& spec)
{
    const auto invalid = validateImageInput(parameters, spec);
    if (!invalid.isEmpty())
        throw std::runtime_error(invalid.toStdString());
    const bool aligned = flag(parameters, spec.alignedKey, spec.alignedDefault);
    if (aligned && context.alignmentFailed) {
        outcome.state = NodeState::Skipped;
        outcome.error = "定位失败，未执行图像算子";
        outcome.input["source"] = "未执行：上游定位失败";
        return std::nullopt;
    }
    const cv::Mat& source = aligned ? context.aligned : context.request.image;
    if (source.empty())
        throw std::runtime_error("图像输入为空");
    QRect region(0, 0, source.cols, source.rows);
    if (!flag(parameters, spec.fullImageKey, spec.fullImageDefault)) {
        QRectF selected;
        const auto saved = parameters.value(spec.roiKey).toArray();
        if (saved.size() == 4)
            selected = QRectF(saved[0].toDouble(), saved[1].toDouble(),
                saved[2].toDouble(), saved[3].toDouble());
        else if (spec.fallbackToReference)
            selected = context.request.reference.referenceRect;
        if (selected.isEmpty() || selected.x() < 0 || selected.y() < 0
            || selected.right() > source.cols || selected.bottom() > source.rows)
            throw std::runtime_error("区域处理需要有效且未越界的图像区域");
        region = selected.toAlignedRect();
        if (region.isEmpty() || !QRect(0, 0, source.cols, source.rows).contains(region))
            throw std::runtime_error("图像区域越界");
    }
    const cv::Mat transform = aligned ? context.alignedToImage : cv::Mat();
    cv::Mat toImage = transform.empty()
        ? (cv::Mat_<double>(2, 3) << 1, 0, 0, 0, 1, 0) : transform.clone();
    toImage.at<double>(0, 2) += toImage.at<double>(0, 0) * region.x()
        + toImage.at<double>(0, 1) * region.y();
    toImage.at<double>(1, 2) += toImage.at<double>(1, 0) * region.x()
        + toImage.at<double>(1, 1) * region.y();
    outcome.input["source"] = aligned && !transform.empty() ? "定位校正图" : "相机原图";
    outcome.input["width"] = region.width();
    outcome.input["height"] = region.height();
    outcome.input["roi"] = QJsonArray { region.x(), region.y(), region.width(), region.height() };
    return OperatorImageInput { source(cv::Rect(region.x(), region.y(), region.width(), region.height())),
        region, toImage };
}
