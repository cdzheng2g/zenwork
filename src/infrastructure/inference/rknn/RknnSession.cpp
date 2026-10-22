#include "RknnSession.h"
#include <QFile>
#include <QFileInfo>
#include <QDateTime>
#include <limits>
#include <algorithm>
#include <stdexcept>
#ifdef HAVE_RKNN
#include "rknn_api.h"
#endif

namespace {
void require(bool ok, const QString& message)
{
    if (!ok)
        throw std::runtime_error(message.toStdString());
}
#ifdef HAVE_RKNN
void checked(int result, const QString& operation)
{
    require(result == RKNN_SUCC, operation + QStringLiteral("（RKNN %1）").arg(result));
}
TensorInfo describe(const rknn_tensor_attr& attr)
{
    require(attr.n_dims > 0 && attr.n_dims <= RKNN_MAX_DIMS && attr.n_elems > 0,
        "不支持空张量或未确定的动态尺寸");
    TensorInfo info;
    info.name = QString::fromUtf8(attr.name,
        int(std::find(attr.name, attr.name + sizeof(attr.name), '\0') - attr.name));
    info.nativeType = get_type_string(attr.type);
    info.quantization = get_qnt_type_string(attr.qnt_type);
    info.shape.assign(attr.dims, attr.dims + attr.n_dims);
    for (auto dimension : info.shape)
        require(dimension > 0, "模型张量尺寸无效");
    info.elements = attr.n_elems;
    info.layout = attr.fmt == RKNN_TENSOR_NCHW ? TensorLayout::Nchw
        : attr.fmt == RKNN_TENSOR_NHWC ? TensorLayout::Nhwc : TensorLayout::Other;
    info.zeroPoint = attr.zp;
    info.scale = attr.scale;
    info.fractionalLength = attr.fl;
    return info;
}
rknn_tensor_type inputType(TensorInputType type)
{
    switch (type) {
    case TensorInputType::UInt8: return RKNN_TENSOR_UINT8;
    case TensorInputType::Int8: return RKNN_TENSOR_INT8;
    case TensorInputType::Float16: return RKNN_TENSOR_FLOAT16;
    case TensorInputType::Float32: return RKNN_TENSOR_FLOAT32;
    }
    throw std::runtime_error("不支持的输入数据类型");
}
std::size_t elementBytes(TensorInputType type)
{
    switch (type) {
    case TensorInputType::UInt8:
    case TensorInputType::Int8: return 1;
    case TensorInputType::Float16: return 2;
    case TensorInputType::Float32: return 4;
    }
    throw std::runtime_error("不支持的输入数据类型");
}
#endif
}

struct RknnSession::Impl {
    std::vector<TensorInfo> inputInfo, outputInfo;
    QString path;
    qint64 size { 0 }, modified { 0 };
    bool timedOut { false };
#ifdef HAVE_RKNN
    rknn_context context { 0 };
    ~Impl() { if (context) rknn_destroy(context); }
#endif
};

RknnSession::RknnSession() : _impl(std::make_unique<Impl>())
{
    bool valid = false;
    const int value = qEnvironmentVariableIntValue("HANS_RKNN_RUN_TIMEOUT_MS", &valid);
    if (valid && value >= 1 && value <= 600000)
        _runTimeoutMs = value;
}
RknnSession::~RknnSession() = default;
bool RknnSession::isLoaded() const { return !_impl->path.isEmpty(); }
const std::vector<TensorInfo>& RknnSession::inputs() const { return _impl->inputInfo; }
const std::vector<TensorInfo>& RknnSession::outputs() const { return _impl->outputInfo; }

void RknnSession::load(const QString& path)
{
#ifdef HAVE_RKNN
    if (_impl->timedOut)
        throw RknnTimeout("RKNN 会话已超时，禁止重新加载，请重启应用恢复设备状态");
    const QFileInfo fileInfo(path);
    require(fileInfo.isFile() && fileInfo.size() > 0, "模型文件不存在或为空：" + path);
    const auto absolute = fileInfo.absoluteFilePath();
    if (_impl->path == absolute && _impl->size == fileInfo.size()
        && _impl->modified == fileInfo.lastModified().toMSecsSinceEpoch())
        return;
    QFile file(absolute);
    require(file.open(QIODevice::ReadOnly), "无法读取模型：" + absolute);
    QByteArray bytes = file.readAll();
    require(bytes.size() == fileInfo.size()
            && quint64(bytes.size()) <= std::numeric_limits<uint32_t>::max(), "模型读取不完整或文件过大");
    // Build a replacement first. Failed initialization/query leaves the old session usable.
    auto next = std::make_unique<Impl>();
    checked(rknn_init(&next->context, bytes.data(), uint32_t(bytes.size()), 0, nullptr),
        "模型初始化失败：" + absolute);
    rknn_input_output_num count {};
    checked(rknn_query(next->context, RKNN_QUERY_IN_OUT_NUM, &count, sizeof(count)), "查询模型输入输出失败");
    require(count.n_input > 0 && count.n_output > 0, "模型没有输入或输出");
    for (uint32_t index = 0; index < count.n_input; ++index) {
        rknn_tensor_attr attr {};
        attr.index = index;
        checked(rknn_query(next->context, RKNN_QUERY_INPUT_ATTR, &attr, sizeof(attr)), "查询输入张量失败");
        next->inputInfo.push_back(describe(attr));
    }
    for (uint32_t index = 0; index < count.n_output; ++index) {
        rknn_tensor_attr attr {};
        attr.index = index;
        checked(rknn_query(next->context, RKNN_QUERY_OUTPUT_ATTR, &attr, sizeof(attr)), "查询输出张量失败");
        next->outputInfo.push_back(describe(attr));
    }
    next->path = absolute;
    next->size = fileInfo.size();
    next->modified = fileInfo.lastModified().toMSecsSinceEpoch();
    _impl.swap(next);
#else
    Q_UNUSED(path)
    throw std::runtime_error("此构建没有 RKNN 推理支持");
#endif
}

std::vector<TensorOutput> RknnSession::run(const std::vector<TensorInput>& inputs)
{
#ifdef HAVE_RKNN
    require(isLoaded(), "尚未加载 RKNN 模型");
    if (_impl->timedOut)
        throw RknnTimeout("RKNN 会话已超时，禁止继续使用，请重启应用恢复设备状态");
    require(inputs.size() == _impl->inputInfo.size(), "输入张量数量与模型不一致");
    std::vector<rknn_input> nativeInputs(inputs.size());
    for (std::size_t index = 0; index < inputs.size(); ++index) {
        const auto& source = inputs[index];
        require(source.data && source.bytes > 0
                && source.bytes <= std::numeric_limits<uint32_t>::max()
                && source.bytes == _impl->inputInfo[index].elements * elementBytes(source.type),
            "输入张量缓冲区大小与模型不一致");
        require(source.layout != TensorLayout::Other, "输入布局必须明确为 NCHW 或 NHWC");
        auto& target = nativeInputs[index];
        target.index = uint32_t(index);
        target.buf = const_cast<void*>(source.data);
        target.size = uint32_t(source.bytes);
        target.type = inputType(source.type);
        target.fmt = source.layout == TensorLayout::Nchw ? RKNN_TENSOR_NCHW : RKNN_TENSOR_NHWC;
    }
    checked(rknn_inputs_set(_impl->context, uint32_t(nativeInputs.size()), nativeInputs.data()), "提交推理输入失败");
    rknn_run_extend runOptions {};
    runOptions.timeout_ms = _runTimeoutMs;
    runOptions.fence_fd = -1;
    const int runStatus = rknn_run(_impl->context, &runOptions);
    if (runStatus == RKNN_ERR_TIMEOUT) {
        _impl->timedOut = true;
        throw RknnTimeout("RKNN 推理超时，检测已锁定，请重启应用恢复设备状态");
    }
    checked(runStatus, "模型推理失败");
    std::vector<rknn_output> outputs(_impl->outputInfo.size());
    for (std::size_t index = 0; index < outputs.size(); ++index) {
        outputs[index].index = uint32_t(index);
        outputs[index].want_float = 1;
    }
    const int outputStatus = rknn_outputs_get(_impl->context, uint32_t(outputs.size()), outputs.data(), nullptr);
    if (outputStatus == RKNN_ERR_TIMEOUT) {
        _impl->timedOut = true;
        throw RknnTimeout("RKNN 获取输出超时，检测已锁定，请重启应用恢复设备状态");
    }
    checked(outputStatus, "获取推理输出失败");
    struct OutputLease {
        rknn_context context;
        std::vector<rknn_output>& outputs;
        ~OutputLease() { rknn_outputs_release(context, uint32_t(outputs.size()), outputs.data()); }
    } lease { _impl->context, outputs };
    std::vector<TensorOutput> result;
    for (std::size_t index = 0; index < outputs.size(); ++index) {
        const auto& out = outputs[index];
        const auto& info = _impl->outputInfo[index];
        require(out.buf && out.size % sizeof(float) == 0 && out.size / sizeof(float) == info.elements,
            "推理输出大小与张量属性不一致");
        const auto* values = static_cast<const float*>(out.buf);
        result.push_back({ info, std::vector<float>(values, values + info.elements) });
    }
    return result;
#else
    Q_UNUSED(inputs)
    throw std::runtime_error("此构建没有 RKNN 推理支持");
#endif
}
