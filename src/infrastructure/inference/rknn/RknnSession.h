#pragma once
#include <QString>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

enum class TensorLayout { Nchw, Nhwc, Other };
enum class TensorInputType { UInt8, Int8, Float16, Float32 };

struct TensorInfo {
    QString name, nativeType, quantization;
    TensorLayout layout { TensorLayout::Other };
    std::vector<uint32_t> shape;
    std::size_t elements { 0 };
    int zeroPoint { 0 }, fractionalLength { 0 };
    float scale { 0 };
};

struct TensorInput {
    const void* data { nullptr };
    std::size_t bytes { 0 };
    TensorInputType type { TensorInputType::UInt8 };
    TensorLayout layout { TensorLayout::Nhwc };
};

struct TensorOutput {
    TensorInfo info;
    // Owned float32 values, converted by RKNN; no manual dequantization is needed.
    std::vector<float> values;
};

// A session belongs to one worker thread. No QWidget or SDK types in the public API.
class RknnSession final {
public:
    RknnSession();
    ~RknnSession();
    RknnSession(const RknnSession&) = delete;
    RknnSession& operator=(const RknnSession&) = delete;

    void load(const QString& path);
    bool isLoaded() const;
    const std::vector<TensorInfo>& inputs() const;
    const std::vector<TensorInfo>& outputs() const;
    std::vector<TensorOutput> run(const std::vector<TensorInput>& inputs);

private:
    struct Impl;
    std::unique_ptr<Impl> _impl;
};
