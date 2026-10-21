#include "BrightnessModule.h"
#include <opencv2/imgproc.hpp>

namespace {
 void execute(OperatorRunContext& context,
             const QJsonObject& parameters,
             OperatorOutcome& outcome){
    const bool aligned = parameters ["useAligned"].toBool();
    if(aligned && context.alignmentFailed){
        outcome.state=NodeState::Skipped;
        outcome.error = "定位失败，未执行灰度统计";
        return;
    }
    const cv::Mat& image = aligned ? context.aligned : context.request.image;
    cv::Mat gray;
    if (image.channels()==1) gray = image;
    else cv::cvtColor(image,gray,cv::COLOR_BGR2GRAY);
    outcome.input["source"] = aligned ? "定位校正图" : "相机原图";
    outcome.fields["mean"] = cv::mean(gray)[0];
    outcome.state = NodeState::Ok;
}
}

OperatorDefinition brightnessDefinition()
{

    OperatorDefinition d;
    d.kind="brightness";
    d.name="平均灰度";
    d.symbol="灰";
    d.parameters={boolParameter("useAligned","使用定位校正图",true)};
    d.outputs = {{"mean", "平均灰度", ParameterKind::Number, ""}};
    d.execute=&execute;
   d.defaultRules = [](const QJsonObject&) {
        return QJsonArray{QJsonObject{
            {"type", "number_range"}, {"field", "mean"},
            {"parameters", QJsonObject{{"minimum", 80}, {"maximum", 200}}}
        }};
    };

    return d;
}