#include "modules/operators/det/decoders/yolov5/YoloV5Decoder.h"
#include <QCoreApplication>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <functional>
#include <limits>

namespace {
std::vector<TensorOutput> heads(bool logits = false)
{
    std::vector<TensorOutput> result;
    for (const uint32_t grid : {8u, 4u, 2u}) {
        TensorOutput output;
        output.info.layout = TensorLayout::Nchw;
        output.info.shape = {1, 21, grid, grid}; // Two classes, three anchors.
        output.info.elements = std::size_t(21) * grid * grid;
        output.values.assign(output.info.elements, logits ? -20.0f : 0.0f);
        result.push_back(output);
    }
    return result;
}
void candidate(std::vector<TensorOutput>& outputs, bool logits = false, int col = 2)
{
    auto& output = outputs[0];
    const std::size_t area = 64, position = 3 * 8 + col;
    const float values[] {.5f, .5f, .5f, .5f, .8f, .2f, .9f};
    for (int channel = 0; channel < 7; ++channel) {
        const float value = values[channel];
        output.values[std::size_t(channel) * area + position]
            = logits ? std::log(value / (1.0f - value)) : value;
    }
}
bool rejects(const std::function<void()>& operation)
{
    try { operation(); } catch (const std::exception&) { return true; }
    return false;
}
}

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);
    int failures = 0;
    auto check = [&](bool ok, const char* label) {
        printf("[%s] %s\n", ok ? "PASS" : "FAIL", label);
        failures += !ok;
    };
    auto near = [](double a, double b) { return std::abs(a - b) < 1e-5; };
    DetectionDecodeContext context {{64, 64}, .5};
    YoloV5Decoder decoder;
    auto outputs = heads();
    candidate(outputs);
    const auto decoded = decoder.decode(outputs, context);
    check(decoded.objects.size() == 1, "one known candidate survives confidence filtering");
    if (decoded.objects.empty()) return 1;
    const auto object = decoded.objects.front();
    check(object.classId == 1 && near(object.confidence, .72), "argmax class and objectness times class probability");
    check(near(object.box.x(), 15) && near(object.box.y(), 21.5)
            && near(object.box.width(), 10) && near(object.box.height(), 13),
        "anchor/grid decoding stays in model-input pixel coordinates");
    check(object.label.isEmpty() && decoded.nmsPolicy == DetectionNmsPolicy::Required,
        "decoder leaves label mapping and NMS to the common pipeline");
    check(decoder.decode(heads(), context).objects.empty(), "no target returns an empty valid batch");

    std::reverse(outputs.begin(), outputs.end());
    auto reordered = decoder.decode(outputs, context);
    check(reordered.objects.size() == 1 && reordered.objects[0].box == object.box,
        "anchor group is selected by stride rather than output order");

    auto logits = heads(true);
    candidate(logits, true);
    YoloV5Decoder logitsDecoder(QJsonObject{{"outputsAreLogits", true}});
    const auto logitsResult = logitsDecoder.decode(logits, context);
    check(logitsResult.objects.size() == 1 && near(logitsResult.objects[0].confidence, .72)
            && near(logitsResult.objects[0].box.width(), 10),
        "explicit logits mode applies sigmoid exactly once");

    const QJsonArray customAnchors {
        QJsonArray{QJsonArray{20,26}, QJsonArray{16,30}, QJsonArray{33,23}},
        QJsonArray{QJsonArray{30,61}, QJsonArray{62,45}, QJsonArray{59,119}},
        QJsonArray{QJsonArray{116,90}, QJsonArray{156,198}, QJsonArray{373,326}}
    };
    YoloV5Decoder custom(QJsonObject{{"anchors", customAnchors}});
    outputs = heads();
    candidate(outputs);
    const auto customized = custom.decode(outputs, context);
    check(customized.objects.size() == 1 && near(customized.objects[0].box.width(), 20)
            && near(customized.objects[0].box.height(), 26), "custom model anchors are honored");

    candidate(outputs, false, 3);
    check(decoder.decode(outputs, context).objects.size() == 2,
        "decoder returns candidates without silently running NMS");
    outputs = heads();
    outputs[0].values.pop_back();
    check(rejects([&]{ decoder.decode(outputs, context); }), "short output buffer rejected before indexed reads");
    outputs = heads();
    outputs[1] = outputs[0];
    check(rejects([&]{ decoder.decode(outputs, context); }), "duplicate scale rejected");
    outputs = heads();
    outputs[1].info.shape[1] = 18;
    outputs[1].info.elements = 0;
    check(rejects([&]{ decoder.decode(outputs, context); }), "different class counts rejected");
    outputs = heads();
    outputs[0].info.layout = TensorLayout::Nhwc;
    check(rejects([&]{ decoder.decode(outputs, context); }), "unsupported output layout rejected");
    outputs = heads();
    candidate(outputs);
    outputs[0].values[4 * 64 + 3 * 8 + 2] = std::numeric_limits<float>::quiet_NaN();
    check(rejects([&]{ decoder.decode(outputs, context); }), "non-finite objectness rejected");
    check(rejects([&]{ decoder.decode(heads(), {{0,64}, .5}); }), "zero model size rejected");
    check(rejects([&]{ decoder.decode(heads(), {{64,64}, 1.1}); }), "invalid score threshold rejected");
    check(rejects([&]{ YoloV5Decoder bad(QJsonObject{{"outputsAreLogits", 1}}); }),
        "wrong activation option type rejected");
    check(rejects([&]{ YoloV5Decoder bad(QJsonObject{{"anchors", QJsonArray{1,2,3}}}); }),
        "malformed anchor settings rejected");
    return failures ? 1 : 0;
}
