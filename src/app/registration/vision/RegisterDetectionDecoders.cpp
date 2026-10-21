#include "application/vision/detection/DetectionDecoderRegistry.h"
#include "modules/operators/det/decoders/yolov5/YoloV5Decoder.h"

const QList<DetectionDecoderDefinition>& DetectionDecoderRegistry::definitions()
{
    static const QList<DetectionDecoderDefinition> definitions {
        yoloV5DecoderDefinition()
    };
    return definitions;
}
