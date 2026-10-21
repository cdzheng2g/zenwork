#pragma once
#include "application/vision/detection/DetectionDecoder.h"
#include <QJsonObject>

struct DetectionDecoderDefinition;
DetectionDecoderDefinition yoloV5DecoderDefinition();

class YoloV5Decoder final : public DetectionDecoder {
public:
    explicit YoloV5Decoder(const QJsonObject& parameters = {});
    void validate(
        const std::vector<TensorInfo>& outputs) const override;
        
    DecodedDetections decode(
        const std::vector<TensorOutput>& outputs,
        const DetectionDecodeContext& context) const override;
private:
    bool _outputsAreLogits {false};
    float _anchors[3][3][2] {};
};
 
