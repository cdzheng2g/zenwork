#include "TemplateModule.h"
#include "TemplateOperator.h"
OperatorDefinition templateDefinition()
{
    OperatorDefinition d;
    d.kind = "template";
    d.name = "模板定位";
    d.symbol = "定";
    d.execute = &executeTemplateOperator;
    d.producesAlignment = true;
    // The current pipeline has one shared alignment coordinate system.
    d.allowsMultiple = false;
    d.outputs = { { "score", "匹配分数", ParameterKind::Number, { } },
        { "dx", "X 偏移", ParameterKind::Number, "px" }, { "dy", "Y 偏移", ParameterKind::Number, "px" },
        { "angle", "旋转角度", ParameterKind::Number, "°" } };
    return d;
}
