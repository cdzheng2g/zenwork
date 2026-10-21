#include "application/operators/OperatorRegistry.h"
#include "modules/operators/ocr/OcrModule.h"
#include "modules/operators/template_match/TemplateModule.h"
#include "modules/operators/brightness/BrightnessModule.h"
#include "modules/operators/det/DetModule.h"

const QList<OperatorDefinition>& OperatorRegistry::definitions()
{
    static const QList<OperatorDefinition> operators { 
        templateDefinition(), 
        ocrDefinition(),
        brightnessDefinition(),
        detectionDefinition()
    };
    return operators;
}
