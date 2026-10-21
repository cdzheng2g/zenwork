#include "application/vision/detection/DetectionEngine.h"
#include "application/InspectionRunner.h"
#include "application/operators/OperatorRegistry.h"
#include "application/scheme/SchemePackage.h"
#include <QCoreApplication>
#include <QTemporaryDir>
#include <QFile>
#include <cmath>
#include <cstdio>
#include <functional>
#include <stdexcept>

namespace {
bool emptyOutput = false;
TensorLayout modelLayout = TensorLayout::Nchw;
TensorInputType lastType;
cv::Vec3f submittedPixel;
int executions = 0;

std::vector<TensorOutput> detectionHeads()
{
    std::vector<TensorOutput> outputs;
    for (uint32_t grid : {8u, 4u, 2u}) {
        TensorOutput output;
        output.info.layout = TensorLayout::Nchw;
        output.info.shape = {1, 21, grid, grid};
        output.info.elements = 21 * grid * grid;
        output.values.assign(output.info.elements, 0);
        outputs.push_back(output);
    }
    if (!emptyOutput) {
        // Two overlapping anchors predict the same object. Only one should survive NMS.
        for (int anchor = 0; anchor < 2; ++anchor) {
            const float w = anchor == 0 ? .5f : std::sqrt(10.0f / 16.0f) / 2;
            const float h = anchor == 0 ? .5f : std::sqrt(13.0f / 30.0f) / 2;
            const float values[] {.5f, .5f, w, h, anchor == 0 ? .8f : .7f, .2f, .9f};
            for (int channel = 0; channel < 7; ++channel)
                outputs[0].values[(anchor * 7 + channel) * 64 + 3 * 8 + 2] = values[channel];
        }
    }
    return outputs;
}
bool rejects(const std::function<void()>& operation)
{
    try { operation(); } catch (const std::exception&) { return true; }
    return false;
}
}

// Link-time replacement of the session only. Engine, decoder, module and rules are production code.
struct RknnSession::Impl {
    QString path;
    std::vector<TensorInfo> inputs, outputs;
};
RknnSession::RknnSession() : _impl(std::make_unique<Impl>()) {}
RknnSession::~RknnSession() = default;
bool RknnSession::isLoaded() const { return !_impl->path.isEmpty(); }
const std::vector<TensorInfo>& RknnSession::inputs() const { return _impl->inputs; }
const std::vector<TensorInfo>& RknnSession::outputs() const { return _impl->outputs; }
void RknnSession::load(const QString& path)
{
    if (!QFile::exists(path)) throw std::runtime_error("missing model");
    _impl->path = path;
    TensorInfo input;
    input.layout = modelLayout;
    input.shape = modelLayout == TensorLayout::Nchw ? std::vector<uint32_t>{1,3,64,64}
        : std::vector<uint32_t>{1,64,64,3};
    input.elements = 64 * 64 * 3;
    _impl->inputs = {input};
    _impl->outputs.clear();
    for (const auto& output : detectionHeads()) _impl->outputs.push_back(output.info);
}
std::vector<TensorOutput> RknnSession::run(const std::vector<TensorInput>& inputs)
{
    if (inputs.size() != 1 || inputs[0].layout != TensorLayout::Nhwc)
        throw std::runtime_error("wrong submission layout");
    const auto& input = inputs.front();
    lastType = input.type;
    const std::size_t index = (16 * 64 + 2) * 3;
    const std::size_t bytes = input.type == TensorInputType::UInt8 ? 1
        : input.type == TensorInputType::Float16 ? 2 : 4;
    if (input.bytes != 64 * 64 * 3 * bytes) throw std::runtime_error("wrong input bytes");
    for (int channel = 0; channel < 3; ++channel) {
        submittedPixel[channel] = input.type == TensorInputType::UInt8
            ? static_cast<const unsigned char*>(input.data)[index + channel]
            : input.type == TensorInputType::Float16
                ? float(static_cast<const cv::float16_t*>(input.data)[index + channel])
                : static_cast<const float*>(input.data)[index + channel];
    }
    ++executions;
    return detectionHeads();
}

int main(int argc, char** argv)
{
    QTemporaryDir storage;
    qputenv("XDG_DATA_HOME", storage.path().toUtf8());
    QCoreApplication app(argc, argv);
    int failures = 0;
    auto check = [&](bool ok, const char* label) {
        printf("[%s] %s\n", ok ? "PASS" : "FAIL", label);
        failures += !ok;
    };
    auto near = [](double a, double b) { return std::abs(a-b) < 1e-5; };
    auto file = [&](const QString& name, const QByteArray& bytes) {
        const auto path = storage.filePath(name);
        QFile output(path);
        if (!output.open(QIODevice::WriteOnly) || output.write(bytes) != bytes.size())
            throw std::runtime_error("cannot prepare fixture");
        return path;
    };
    const auto model = file("det.rknn", "fake session fixture");
    const auto labels = file("labels.txt", "bolt\nnut\n");
    DetectionOptions options;
    options.modelPath = model;
    options.decoderId = "yolov5";
    options.labels = QStringList{"bolt", "nut"};
    DetectionEngine engine;
    const cv::Mat image(32,64,CV_8UC3,cv::Scalar(12,34,56));
    auto objects = engine.detect(image, options);
    check(objects.size() == 1 && objects[0].label == "nut" && near(objects[0].confidence,.72),
        "registered decoder, label mapping and class-aware NMS form a complete pipeline");
    if (objects.empty()) return 1;
    check(near(objects[0].box.x(),15) && near(objects[0].box.y(),5.5)
        && near(objects[0].box.width(),10) && near(objects[0].box.height(),13),
        "letterbox padding is removed before returning source-image boxes");
    check(submittedPixel == cv::Vec3f(56,34,12) && lastType == TensorInputType::UInt8,
        "RKNN receives contiguous NHWC RGB bytes, independent of native model layout");
    check(image.at<cv::Vec3b>(0,0) == cv::Vec3b(12,34,56),
        "color conversion never mutates the shared inspection image");
    modelLayout = TensorLayout::Nhwc;
    options.preprocess.convertToRgb = false;
    check(engine.detect(image,options).size() == 1 && submittedPixel == cv::Vec3f(12,34,56),
        "NHWC models and BGR-preserving preprocessing are supported");
    options.preprocess.inputType = TensorInputType::Float32;
    options.preprocess.scale = 1.0 / 255;
    engine.detect(image,options);
    check(lastType == TensorInputType::Float32 && near(submittedPixel[0],12.0/255),
        "FLOAT32 input applies the configured normalization");
    options.preprocess.inputType = TensorInputType::Float16;
    engine.detect(image,options);
    check(lastType == TensorInputType::Float16 && std::abs(submittedPixel[0]-12.0/255) < .0001,
        "FLOAT16 input retains normalized values");
    options.preprocess.inputType = TensorInputType::UInt8;
    check(rejects([&]{ engine.detect(image,options); }), "UINT8 never silently ignores custom normalization");
    options.preprocess = {};
    options.preprocess.resizeMode = ImageResizeMode::Stretch;
    objects = engine.detect(image,options);
    check(near(objects[0].box.y(),10.75) && near(objects[0].box.height(),6.5),
        "stretch restores independent horizontal and vertical scales");
    options.labels = QStringList{"wrong"};
    check(rejects([&]{ engine.detect(image,options); }), "mismatched class dictionaries are rejected");
    options.labels.clear();
    check(engine.detect(image,options)[0].label == QStringLiteral("类别 1"),
        "callers without a label dictionary receive an explicit class identifier");
    check(rejects([&]{ DetectionDecoderRegistry::create("missing",{}); }), "unregistered decoder is an error");
    check(rejects([&]{ DetectionDecoderRegistry::create("yolov5",{{"anchorsJson","bad"}}); }),
        "invalid model-specific parameters are caught by the registered factory");

    auto tool = OperatorRegistry::makeTool("detection");
    tool.parameters["modelPath"] = model;
    tool.parameters["labelsPath"] = labels;
    check(tool.kind == "detection" && OperatorRegistry::validate(tool).isEmpty(),
        "detection is a real registered operator with valid defaults");
    auto bad = tool;
    bad.parameters["decoder.yolov5.anchorsJson"] = "invalid";
    check(!OperatorRegistry::validate(bad).isEmpty(), "operator validation reaches decoder-owned settings");
    InspectionRequest request;
    request.scheme = "detector";
    request.image = cv::Mat(80,100,CV_8UC3,cv::Scalar(12,34,56));
    tool.parameters["fullImage"] = false;
    tool.parameters["roi"] = QJsonArray{10,20,64,32};
    request.tools = {tool};
    auto result = runInspection(request);
    auto node = result["tools"].toArray()[0].toObject();
    auto fields = node["fields"].toObject();
    const auto corner = fields["regions"].toArray()[0].toObject()["polygon"].toArray()[0].toArray();
    check(result["status"] == "OK" && fields["count"] == 1 && fields["labels"] == "nut"
        && near(corner[0].toDouble(),25) && near(corner[1].toDouble(),25.5),
        "inspection rules and preview polygons use the restored ROI coordinates");
    emptyOutput = true;
    result = runInspection(request);
    node = result["tools"].toArray()[0].toObject();
    check(result["status"] == "NG" && node["fields"].toObject()["count"] == 0,
        "no target is a valid zero-count result and the configured rule yields NG");
    emptyOutput = false;
    auto malformed = tool;
    malformed.parameters["labelsPath"] = file("short.txt","wrong\n");
    request.tools = {malformed};
    check(runInspection(request)["status"] == "ERROR", "incompatible labels yield ERROR rather than false OK");
    malformed = tool;
    malformed.parameters["modelPath"] = storage.filePath("missing.rknn");
    request.tools = {malformed};
    check(runInspection(request)["status"] == "ERROR", "missing models remain visible inspection errors");

    request.tools = {tool};
    SchemePackage package;
    package.name = "detector";
    package.tools = request.tools;
    QString error;
    const auto path = storage.filePath("detector.yaml");
    SchemePackage loaded;
    check(package.save(path,&error,true) && SchemePackage::load(path,loaded,&error)
        && loaded.tools.size() == 1 && loaded.resources.size() == 2,
        "detector model and labels follow the existing portable YAML resource lifecycle");
    return failures ? 1 : 0;
}
