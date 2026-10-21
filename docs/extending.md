# 如何增加功能

## RKNN 模型与图像算子

添加目标检测模型请先读 [adding-detection.md](adding-detection.md)，可编译的四文件模板在 `docs/examples/detection/`。
RKNN 会话在 `infrastructure/inference/rknn`，通用图像输入、缩放、检测筛选与坐标映射在 `application/vision`。
声明 `OperatorDefinition::imageInput` 后自动使用 `ImageOperatorEditor`，无需为常规 ROI 和模型选择新建页面。
`fileParameter()` 提供文件输入与选择，`advanced` 属性控制在图像编辑器中是否折叠；模型资源仍需在 `resources` 中登记。

## 新增普通算子

以“平均灰度”为例。新建目录 `src/modules/operators/brightness/`，新增两个文件。

`BrightnessModule.h`：

```cpp
#pragma once
#include "application/operators/OperatorRegistry.h"
OperatorDefinition brightnessDefinition();
```

`BrightnessModule.cpp`：

```cpp
#include "BrightnessModule.h"
#include <opencv2/imgproc.hpp>

namespace {
void execute(OperatorRunContext& context, const QJsonObject& parameters,
             OperatorOutcome& outcome)
{
    const bool aligned = parameters["useAligned"].toBool();
    if (aligned && context.alignmentFailed) {
        outcome.state = NodeState::Skipped;
        outcome.error = "定位失败，未执行灰度统计";
        return;
    }
    const cv::Mat& image = aligned ? context.aligned : context.request.image;
    cv::Mat gray;
    if (image.channels() == 1) gray = image;
    else cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
    outcome.input["source"] = aligned ? "定位校正图" : "相机原图";
    outcome.fields["mean"] = cv::mean(gray)[0];
    outcome.state = NodeState::Ok;
}
}

OperatorDefinition brightnessDefinition()
{
    OperatorDefinition d;
    d.kind = "brightness";
    d.name = "平均灰度";
    d.symbol = "灰";
    d.parameters = {boolParameter("useAligned", "使用定位校正图", true)};
    d.outputs = {{"mean", "平均灰度", ParameterKind::Number, ""}};
    d.execute = &execute;
    d.defaultRules = [](const QJsonObject&) {
        return QJsonArray{QJsonObject{
            {"type", "number_range"}, {"field", "mean"},
            {"parameters", QJsonObject{{"minimum", 80}, {"maximum", 200}}}
        }};
    };
    return d;
}
```

然后修改两个装配文件：

1. `src/app/registration/vision/RegisterOperators.cpp`：包含头文件，将 `brightnessDefinition()` 加入列表。
2. `src/Modules.cmake`：在 `hans_operators` 中加入 `modules/operators/brightness/BrightnessModule.cpp`。

这样会得到算子库入口、独立参数保存、自动生成的开关界面、数值范围判断、结果表字段和通用 YAML 读写。
不需要修改 `WorkbenchWidget`、`InspectionRunner`、`SchemeCatalog` 或通信协议的通用调度。
平均灰度已由用户按此指南加入当前项目。下面的示例说明简单算子的实现方式。

通常是 **新增 2 个业务文件，修改 2 个装配文件**。算法自身有多个实现文件、需要模型资源或图像编辑界面时再增加。
若需要画 ROI 等专用交互，实现 `OperatorEditor`，在 `app/registration/ui/RegisterOperatorEditors.cpp` 注册工厂，
把专用编辑器加入 `hans_operator_editors`。保持 `configuration()/setConfiguration()` 对称，避免切换实例时丢参数。

## OCR 新增判定依据

已有“文字非空、长度、相等、数值范围”。使用这些依据只需在算子的“判定条件”中添加，不改代码。

如果新增“文字包含”：

1. 新建 `modules/rules/TextContainsRule.h/.cpp`，返回一个 `RuleDefinition`。
2. 定义 `type="text_contains"`、`inputKind=ParameterKind::Text`、期望文字等参数。
3. `evaluate` 接收输出值和参数，返回 `RuleOutcome`；额外参数关系通过 `validate` 校验。
4. 在 `app/registration/rules/RegisterRules.cpp` 加入该规则，并在 `hans_rules` 中加入 cpp。

规则类型与输出类型匹配后，编辑器会自动提供该规则和参数表单。无需改 OCR 模型、OCR 执行函数或结果发送逻辑。
如果新增依据需要 OCR 计算一个全新指标，则还要在 OCR 执行输出中产生该字段，并在 `OcrModule.cpp` 声明输出字段。

## 新增通信协议

底层连接和报文放 `infrastructure/communication/`。模块放 `modules/communication/<协议名>/`，
提供 `CommunicationModule` 的 id、名称、参数描述、start/stop/status/publish/observe。
在 `app/registration/communication/RegisterCommunication.cpp` 登记，并把实现加入 `hans_protocols`。

通信参数界面、方案中的参数持久化、启停调度和结果派发会复用该描述，不在工作台中增加协议 switch。
新增的协议如果还承担输入触发，需要单独接到 `InspectionService` 的触发准入；这不等同于新增一个结果输出通道。

## 修改公共控件

- 红色主题、间距、高度：`ui/theme/AppTheme.h`。
- 滑动条与数值框联动：`ui/components/NumericParameterField`。
- 开关外观：`ui/components/ToggleField.h`。
- 根据字段描述生成表单：`ui/components/ParameterForm`。

模块参数中的字段 key 是保存协议的一部分。改显示名称不会影响旧方案；改 key、类型或语义需要增加迁移逻辑。
新增模块要验证参数保存/加载、异常输入、OK/NG/ERROR，以及真实算法或协议行为；通用机制的重复验证无需在每个模块重写。
