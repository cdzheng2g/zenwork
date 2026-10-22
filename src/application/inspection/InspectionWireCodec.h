#pragma once
#include "application/InspectionRunner.h"
#include "application/scheme/SchemeCodec.h"
#include <QJsonArray>
#include <opencv2/imgcodecs.hpp>
#include <stdexcept>

namespace InspectionWireCodec {
inline QString encodeImage(const cv::Mat& image)
{
    if (image.empty()) return {};
    std::vector<uchar> bytes;
    if (!cv::imencode(".png", image, bytes)) throw std::runtime_error("无法编码检测图像");
    return QString::fromLatin1(QByteArray(reinterpret_cast<const char*>(bytes.data()), int(bytes.size())).toBase64());
}
inline cv::Mat decodeImage(const QJsonValue& value)
{
    if (!value.isString()) throw std::runtime_error("检测图像格式无效");
    if (value.toString().isEmpty()) return {};
    const auto bytes = QByteArray::fromBase64(value.toString().toLatin1());
    const std::vector<uchar> encoded(bytes.begin(), bytes.end());
    auto image = cv::imdecode(encoded, cv::IMREAD_UNCHANGED);
    if (image.empty() || image.total() > 32 * 1024 * 1024)
        throw std::runtime_error("检测图像为空或过大");
    return image;
}
inline QJsonObject encode(const InspectionRequest& request)
{
    const auto r = request.reference.referenceRect;
    return {{"scheme", request.scheme}, {"settings", SchemeCodec::encodeSettings(request.settings)},
        {"tools", SchemeCodec::encodeTools(request.tools)}, {"image", encodeImage(request.image)},
        {"referenceImage", encodeImage(request.reference.image)},
        {"referenceName", request.reference.modelName},
        {"referencePath", request.reference.imagePath},
        {"referenceRect", QJsonArray {r.x(), r.y(), r.width(), r.height()}},
        {"trace", request.trace.toJson()}};
}
inline InspectionRequest decode(const QJsonObject& value)
{
    InspectionRequest result;
    if (!value["settings"].isObject() || !value["tools"].isArray() || !value["trace"].isObject())
        throw std::runtime_error("检测请求字段无效");
    result.scheme = value["scheme"].toString();
    result.settings = SchemeCodec::decodeSettings(value["settings"].toObject());
    result.tools = SchemeCodec::decodeTools(value["tools"].toArray());
    result.image = decodeImage(value["image"]);
    result.reference.image = decodeImage(value["referenceImage"]);
    result.reference.modelName = value["referenceName"].toString();
    result.reference.imagePath = value["referencePath"].toString();
    const auto rect = value["referenceRect"].toArray();
    if (rect.size() != 4) throw std::runtime_error("检测基准区域无效");
    for (const auto v : rect) if (!v.isDouble()) throw std::runtime_error("检测基准区域格式无效");
    result.reference.referenceRect = {rect[0].toDouble(), rect[1].toDouble(), rect[2].toDouble(), rect[3].toDouble()};
    result.trace = InspectionTrace::fromJson(value["trace"].toObject());
    return result;
}
}
