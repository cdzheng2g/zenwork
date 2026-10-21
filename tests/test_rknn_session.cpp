#include "infrastructure/inference/rknn/RknnSession.h"
#include "rknn_api.h"
#include <QCoreApplication>
#include <QTemporaryDir>
#include <QFile>
#include <cstring>
#include <map>
#include <cstdio>
#include <stdexcept>

namespace {
std::map<rknn_context, char> contexts;
rknn_context nextContext = 0;
int initializations = 0, releases = 0, submitted = 0;
bool malformedOutput = false;
}

// Deterministic SDK substitute: exercise the real session implementation without an NPU.
extern "C" {
int rknn_init(rknn_context* context, void* model, uint32_t, uint32_t, rknn_init_extend*)
{
    ++initializations;
    *context = ++nextContext;
    contexts[*context] = static_cast<char*>(model)[0];
    return contexts[*context] == 'I' ? -1 : 0;
}
int rknn_destroy(rknn_context context) { contexts.erase(context); return 0; }
int rknn_query(rknn_context context, rknn_query_cmd cmd, void* info, uint32_t)
{
    if (cmd == RKNN_QUERY_IN_OUT_NUM) {
        *static_cast<rknn_input_output_num*>(info) = {2, 3};
        return 0;
    }
    if (contexts[context] == 'Q')
        return -2;
    auto& attr = *static_cast<rknn_tensor_attr*>(info);
    const auto index = attr.index;
    attr = {};
    attr.index = index;
    std::strcpy(attr.name, cmd == RKNN_QUERY_INPUT_ATTR ? "input" : "output");
    attr.n_dims = 4;
    attr.dims[0] = 1;
    attr.dims[1] = 1;
    attr.dims[2] = 2;
    attr.dims[3] = 2;
    attr.n_elems = cmd == RKNN_QUERY_INPUT_ATTR ? 4 : 2;
    if (cmd == RKNN_QUERY_OUTPUT_ATTR) {
        attr.n_dims = 2;
        attr.dims[0] = 1;
        attr.dims[1] = 2;
    }
    attr.fmt = RKNN_TENSOR_NCHW;
    attr.type = RKNN_TENSOR_INT8;
    attr.qnt_type = RKNN_TENSOR_QNT_AFFINE_ASYMMETRIC;
    attr.scale = 0.25f;
    attr.zp = -3;
    return 0;
}
int rknn_inputs_set(rknn_context, uint32_t count, rknn_input inputs[])
{
    submitted = int(count);
    return count == 2 && inputs[0].size == 4 && inputs[1].index == 1 ? 0 : -3;
}
int rknn_run(rknn_context, rknn_run_extend*) { return 0; }
int rknn_outputs_get(rknn_context, uint32_t count, rknn_output outputs[], rknn_output_extend*)
{
    for (uint32_t i = 0; i < count; ++i) {
        if (!outputs[i].want_float || outputs[i].index != i)
            return -4;
        outputs[i].buf = new float[2] {float(i + 1), float(i + 10)};
        outputs[i].size = malformedOutput && i == 1 ? sizeof(float) : 2 * sizeof(float);
    }
    return 0;
}
int rknn_outputs_release(rknn_context, uint32_t count, rknn_output outputs[])
{
    ++releases;
    for (uint32_t i = 0; i < count; ++i)
        delete[] static_cast<float*>(outputs[i].buf);
    return 0;
}
}

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);
    QTemporaryDir directory;
    int failures = 0;
    auto check = [&](bool ok, const char* label) {
        printf("[%s] %s\n", ok ? "PASS" : "FAIL", label);
        failures += !ok;
    };
    auto file = [&](const QString& name, char marker) {
        const auto path = directory.filePath(name);
        QFile model(path);
        if (!model.open(QIODevice::WriteOnly) || model.write(&marker, 1) != 1)
            throw std::runtime_error("cannot prepare fake model");
        return path;
    };
    const auto good = file("good.rknn", 'A');
    const auto badQuery = file("query.rknn", 'Q');
    const auto badInit = file("init.rknn", 'I');
    const unsigned char bytes[] {1, 2, 3, 4};
    const std::vector<TensorInput> inputs {
        {bytes, sizeof(bytes), TensorInputType::UInt8, TensorLayout::Nhwc},
        {bytes, sizeof(bytes), TensorInputType::UInt8, TensorLayout::Nhwc}
    };
    {
        RknnSession session;
        session.load(good);
        session.load(good);
        check(initializations == 1 && session.inputs().size() == 2 && session.outputs().size() == 3,
            "unchanged model reused; multiple inputs and detection heads exposed");
        auto outputs = session.run(inputs);
        check(submitted == 2 && outputs.size() == 3 && outputs[2].values[0] == 3
                && outputs[0].values[1] == 10 && releases == 1,
            "all outputs are copied before SDK buffers are released");
        check(outputs[0].info.shape == std::vector<uint32_t>({1, 2})
                && outputs[0].info.nativeType == "INT8" && outputs[0].info.zeroPoint == -3,
            "native metadata retained while returned values are float32");
        for (const auto& path : {badQuery, badInit}) {
            bool failed = false;
            try { session.load(path); } catch (const std::exception&) { failed = true; }
            check(failed && contexts.size() == 1 && session.run(inputs).size() == 3,
                "failed replacement destroys partial context and keeps old model usable");
        }
        auto shortInput = inputs;
        shortInput[0].bytes = 3;
        bool rejected = false;
        try { session.run(shortInput); } catch (const std::exception&) { rejected = true; }
        check(rejected, "wrong input byte count rejected before submission");
        malformedOutput = true;
        const int before = releases;
        rejected = false;
        try { session.run(inputs); } catch (const std::exception&) { rejected = true; }
        check(rejected && releases == before + 1, "invalid output also releases every SDK buffer");
    }
    check(contexts.empty(), "session destructor releases the live model");
    return failures ? 1 : 0;
}
