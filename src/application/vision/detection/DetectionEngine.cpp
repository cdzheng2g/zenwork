#include "DetectionEngine.h"
#include "infrastructure/inference/rknn/RknnSessionCache.h"
#include <opencv2/imgproc.hpp>
#include <cmath>
#include <limits>
#include <stdexcept>

namespace {
void require(bool ok, const char* message)
{
    if (!ok) throw std::runtime_error(message);
}

cv::Size modelImageSize(const std::vector<TensorInfo>& inputs)
{
    require(inputs.size() == 1, "此检测引擎需要单图像输入模型");
    const auto& input = inputs.front();
    require(input.shape.size() == 4 && input.shape[0] == 1
        && (input.layout == TensorLayout::Nchw || input.layout == TensorLayout::Nhwc),
        "检测模型需要固定尺寸、batch=1 的 NCHW 或 NHWC 输入");
    for (auto size : input.shape)
        require(size > 0 && size <= uint32_t(std::numeric_limits<int>::max()), "模型输入尺寸无效");
    const bool nchw = input.layout == TensorLayout::Nchw;
    require(input.shape[nchw ? 1 : 3] == 3, "检测模型需要三通道输入");
    return {int(input.shape[nchw ? 3 : 2]), int(input.shape[nchw ? 2 : 1])};
}

cv::Mat tensorImage(const cv::Mat& image, const DetectionPreprocess& options)
{
    require(std::isfinite(options.scale), "输入归一化倍率无效");
    bool identity = options.scale == 1;
    for (int channel = 0; channel < 3; ++channel) {
        require(std::isfinite(options.mean[channel])
            && std::isfinite(options.standardDeviation[channel]) && options.standardDeviation[channel] > 0,
            "输入归一化均值或标准差无效");
        identity = identity && options.mean[channel] == 0 && options.standardDeviation[channel] == 1;
    }
    if (options.inputType == TensorInputType::UInt8) {
        require(identity, "UINT8 输入由 RKNN 模型处理归一化；自定义归一化请使用浮点输入");
        return image.isContinuous() ? image : image.clone();
    }
    require(options.inputType == TensorInputType::Float32 || options.inputType == TensorInputType::Float16,
        "图像检测引擎支持 UINT8/FLOAT32/FLOAT16 输入；INT8 模型请提交 UINT8 图像，由 RKNN 转换");
    cv::Mat result;
    image.convertTo(result, CV_32F, options.scale);
    for (int row = 0; row < result.rows; ++row) {
        auto* pixels = result.ptr<cv::Vec3f>(row);
        for (int col = 0; col < result.cols; ++col) {
            for (int channel = 0; channel < 3; ++channel) {
                const double value = (pixels[col][channel] - options.mean[channel])
                    / options.standardDeviation[channel];
                require(std::isfinite(value) && std::abs(value) <= std::numeric_limits<float>::max(),
                    "输入归一化结果超出浮点范围");
                require(options.inputType != TensorInputType::Float16 || std::abs(value) <= 65504,
                    "输入归一化结果超出 FLOAT16 范围");
                pixels[col][channel] = float(value);
            }
        }
    }
    if (options.inputType == TensorInputType::Float16) result.convertTo(result, CV_16F);
    return result;
}
}

std::vector<DetectedObject> DetectionEngine::detect(const cv::Mat& bgr, const DetectionOptions& options)
{
    require(!bgr.empty() && bgr.type() == CV_8UC3, "目标检测需要 BGR 三通道图像");
    require(std::isfinite(options.confidenceThreshold) && options.confidenceThreshold >= 0
        && options.confidenceThreshold <= 1 && std::isfinite(options.nmsThreshold)
        && options.nmsThreshold >= 0 && options.nmsThreshold <= 1, "检测阈值必须在 0 到 1 之间");
    require(options.preprocess.resizeMode == ImageResizeMode::Letterbox
        || options.preprocess.resizeMode == ImageResizeMode::Stretch, "未知的图像缩放方式");
    for (int channel = 0; channel < 3; ++channel)
        require(std::isfinite(options.preprocess.padding[channel])
            && options.preprocess.padding[channel] >= 0 && options.preprocess.padding[channel] <= 255,
            "填充颜色必须在 0 到 255 之间");

    if (!_decoder || _decoderId != options.decoderId || _decoderParameters != options.decoderParameters) {
        auto decoder = DetectionDecoderRegistry::create(options.decoderId, options.decoderParameters);
        _decoder = std::move(decoder);
        _decoderId = options.decoderId;
        _decoderParameters = options.decoderParameters;
    }
    const auto session = cachedRknnSession(options.modelPath);
    const auto size = modelImageSize(session->inputs());
    _decoder->validate(session->outputs());

    cv::Mat colors;
    if (options.preprocess.convertToRgb) cv::cvtColor(bgr, colors, cv::COLOR_BGR2RGB);
    else colors = bgr;
    const auto prepared = resizeForModel(colors, size, options.preprocess.resizeMode, options.preprocess.padding);
    const auto input = tensorImage(prepared.image, options.preprocess);
    const auto outputs = session->run({{input.data, input.total() * input.elemSize(),
        options.preprocess.inputType, TensorLayout::Nhwc}});
    auto decoded = _decoder->decode(outputs, {size, options.confidenceThreshold, int(options.labels.size())});
    const QRectF bounds(0, 0, bgr.cols, bgr.rows);
    for (auto& object : decoded.objects) {
        require(object.classId >= 0, "检测结果的类别编号无效");
        if (!options.labels.isEmpty()) {
            require(object.classId < options.labels.size(), "类别编号超出标签文件范围");
            object.label = options.labels[object.classId];
        } else if (object.label.isEmpty()) {
            object.label = QStringLiteral("类别 %1").arg(object.classId);
        }
        object.box = prepared.sourceRect(object.box).intersected(bounds);
    }
    require(decoded.nmsPolicy == DetectionNmsPolicy::Required
        || decoded.nmsPolicy == DetectionNmsPolicy::AlreadyApplied, "解码器返回了未知的 NMS 策略");
    // IoU cannot exceed 1, so this branch filters/sorts/caps without suppressing twice.
    return suppressDetections(std::move(decoded.objects), options.confidenceThreshold,
        decoded.nmsPolicy == DetectionNmsPolicy::Required ? options.nmsThreshold : 1.0);
}
