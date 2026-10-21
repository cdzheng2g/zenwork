#include "YoloV5Decoder.h"
#include "application/vision/detection/DetectionDecoderRegistry.h"
#include <QJsonDocument>
#include <cmath>
#include <limits>
#include <stdexcept>

namespace {
constexpr float defaultAnchors[3][3][2] {
    {{10, 13}, {16, 30}, {33, 23}},
    {{30, 61}, {62, 45}, {59, 119}},
    {{116, 90}, {156, 198}, {373, 326}}
};
void require(bool ok, const char* message)
{
    if (!ok) throw std::runtime_error(message);
}
int classCount(const TensorInfo& output)
{
    const auto& s = output.shape;
    require(output.layout == TensorLayout::Nchw && s.size() == 4,
        "YOLOv5 原始检测头需要 NCHW 四维输出");
    require(s[0] == 1 && s[1] > 0 && s[2] > 0 && s[3] > 0, "检测头 batch 或尺寸无效");
    for (auto dimension : s)
        require(dimension <= uint32_t(std::numeric_limits<int>::max()), "检测头尺寸超出范围");
    require(s[1] % 3 == 0 && s[1] / 3 > 5, "通道数不符合三个 anchors 的格式");
    const std::size_t area = std::size_t(s[2]) * s[3];
    require(area <= std::numeric_limits<std::size_t>::max() / s[1], "检测头元素数量溢出");
    require(output.elements == 0 || output.elements == area * s[1], "检测头元素数量与尺寸不一致");
    return int(s[1] / 3) - 5;
}
float toProbability(float value, bool logits)
{
    require(std::isfinite(value), "YOLOv5 输出包含非有限数值");
    if (!logits) return value;
    if (value >= 0) return 1.0f / (1.0f + std::exp(-value));
    const float exponential = std::exp(value);
    return exponential / (1.0f + exponential);
}

std::unique_ptr<DetectionDecoder> createDecoder(const QJsonObject& parameters)
{
    auto options = parameters;
    const auto json = options.take("anchorsJson").toString().trimmed();
    if (!json.isEmpty()) {
        require(!options.contains("anchors"), "anchors 与 anchorsJson 不能同时设置");
        QJsonParseError error;
        const auto document = QJsonDocument::fromJson(json.toUtf8(), &error);
        require(error.error == QJsonParseError::NoError && document.isArray(), "anchors 必须为合法的 JSON 数组");
        options["anchors"] = document.array();
    }
    return std::make_unique<YoloV5Decoder>(options);
}
}

DetectionDecoderDefinition yoloV5DecoderDefinition()
{
    return {"yolov5", &createDecoder, "YOLOv5",
        {boolParameter("outputsAreLogits", "输出需要 sigmoid", false),
         textParameter("anchorsJson", "自定义 anchors（JSON）")}};
}

YoloV5Decoder::YoloV5Decoder(const QJsonObject& parameters)
{
    for (auto it = parameters.begin(); it != parameters.end(); ++it)
        require(it.key() == "outputsAreLogits" || it.key() == "anchors", "未知的 YOLOv5 解码参数");
    if (parameters.contains("outputsAreLogits")) {
        require(parameters["outputsAreLogits"].isBool(), "outputsAreLogits 必须为布尔值");
        _outputsAreLogits = parameters["outputsAreLogits"].toBool();
    }
    const bool custom = parameters.contains("anchors");
    require(!custom || parameters["anchors"].isArray(), "anchors 必须为数组");
    const auto groups = parameters["anchors"].toArray();
    require(!custom || groups.size() == 3, "anchors 需要 stride 8/16/32 三组数据");
    for (int group = 0; group < 3; ++group) {
        const auto pairs = custom ? groups[group].toArray() : QJsonArray();
        require(!custom || pairs.size() == 3, "每个尺度需要三个 anchor");
        for (int anchor = 0; anchor < 3; ++anchor) {
            const auto dimensions = custom ? pairs[anchor].toArray() : QJsonArray();
            require(!custom || dimensions.size() == 2, "anchor 必须包含宽和高");
            for (int axis = 0; axis < 2; ++axis) {
                require(!custom || dimensions[axis].isDouble(), "anchor 宽高必须为数值");
                const double value = custom ? dimensions[axis].toDouble() : defaultAnchors[group][anchor][axis];
                require(std::isfinite(value) && value > 0 && value <= std::numeric_limits<float>::max(),
                    "anchor 宽高必须为有限正数");
                _anchors[group][anchor][axis] = float(value);
            }
        }
    }
}

void YoloV5Decoder::validate(const std::vector<TensorInfo>& outputs) const
{
    require(outputs.size() == 3, "此 YOLOv5 解码器要求三个原始检测头");
    const int expected = classCount(outputs.front());
    for (const auto& output : outputs)
        require(classCount(output) == expected, "三个检测头的类别数量不一致");
}

DecodedDetections YoloV5Decoder::decode(const std::vector<TensorOutput>& outputs,
    const DetectionDecodeContext& context) const
{
    require(context.modelSize.width > 0 && context.modelSize.height > 0, "模型输入尺寸无效");
    require(std::isfinite(context.confidenceThreshold) && context.confidenceThreshold >= 0
            && context.confidenceThreshold <= 1, "置信度阈值必须在 0 到 1 之间");
    std::vector<TensorInfo> metadata;
    for (const auto& output : outputs) metadata.push_back(output.info);
    validate(metadata);
    require(context.expectedClasses >= 0, "期望类别数量无效");
    require(context.expectedClasses == 0 || classCount(metadata.front()) == context.expectedClasses,
        "模型类别数量与标签文件行数不一致，请选择此模型对应的标签文件");
    DecodedDetections result;
    result.nmsPolicy = DetectionNmsPolicy::Required;
    bool seen[3] {false, false, false};

    // 先检查所有缓冲区与尺度，再读取候选框。
    for (const auto& head : outputs) {
        const auto& s = head.info.shape;
        require(head.values.size() == std::size_t(s[1]) * s[2] * s[3], "输出缓冲区大小与尺寸不一致");
        const int gridH = int(s[2]), gridW = int(s[3]);
        require(context.modelSize.width % gridW == 0 && context.modelSize.height % gridH == 0,
            "网格与模型输入尺寸不匹配");
        const int stride = context.modelSize.width / gridW;
        require(stride == context.modelSize.height / gridH, "横纵 stride 必须一致");
        const int group = stride == 8 ? 0 : stride == 16 ? 1 : stride == 32 ? 2 : -1;
        require(group >= 0 && !seen[group], "检测头必须分别对应 stride 8、16、32");
        seen[group] = true;
    }
    for (const auto& head : outputs) {
        const auto& s = head.info.shape;
        const auto& values = head.values;
        const int gridH = int(s[2]), gridW = int(s[3]);
        const int attributes = int(s[1] / 3), numClasses = attributes - 5;
        const int stride = context.modelSize.width / gridW;
        const int group = stride == 8 ? 0 : stride == 16 ? 1 : 2;
        const std::size_t area = std::size_t(gridH) * gridW;
        for (int anchor = 0; anchor < 3; ++anchor) {
            for (int row = 0; row < gridH; ++row) {
                for (int col = 0; col < gridW; ++col) {
                    const std::size_t base = std::size_t(anchor * attributes) * area
                        + std::size_t(row) * gridW + col;
                    const float objectness = toProbability(values[base + 4 * area], _outputsAreLogits);
                    if (objectness < context.confidenceThreshold) continue;
                    int classId = 0;
                    float best = values[base + 5 * area];
                    for (int c = 1; c < numClasses; ++c) {
                        const float value = values[base + std::size_t(5 + c) * area];
                        if (value > best) {
                            best = value;
                            classId = c;
                        }
                    }
                    const float confidence = objectness * toProbability(best, _outputsAreLogits);
                    if (confidence < context.confidenceThreshold) continue;
                    const float px = toProbability(values[base], _outputsAreLogits);
                    const float py = toProbability(values[base + area], _outputsAreLogits);
                    const float pw = toProbability(values[base + 2 * area], _outputsAreLogits);
                    const float ph = toProbability(values[base + 3 * area], _outputsAreLogits);
                    const float centerX = (px * 2.0f - 0.5f + col) * stride;
                    const float centerY = (py * 2.0f - 0.5f + row) * stride;
                    // 与教程一致：先乘 2，再平方并乘 anchor。
                    float width = pw * 2.0f, height = ph * 2.0f;
                    width = width * width * _anchors[group][anchor][0];
                    height = height * height * _anchors[group][anchor][1];
                    require(std::isfinite(centerX) && std::isfinite(centerY) && std::isfinite(width)
                            && std::isfinite(height) && std::isfinite(confidence), "候选框包含非有限数值");
                    if (width <= 0 || height <= 0) continue;
                    DetectedObject object;
                    object.classId = classId;
                    object.confidence = confidence;
                    object.box = QRectF(centerX - width / 2.0f, centerY - height / 2.0f, width, height);
                    result.objects.push_back(object);
                }
            }
        }
    }
    return result;
}
