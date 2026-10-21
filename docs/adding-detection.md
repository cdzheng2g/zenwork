# 扩展目标检测模型

当前目标检测已使用公共引擎，不再复制一套预处理、推理或界面。
先阅读当前 YOLOv5 实现，再按下列规则增加新的解码器。

## 调用关系

```text
DetModule
  → prepareOperatorImage：原图/校正图与 ROI
  → DetectionEngine
      → RknnSession：加载模型和运行推理
      → DetectionDecoderRegistry：按 decoderId 创建解码器
          → YoloV5Decoder：模型专属解码
      → 标签映射、坐标还原、公共 NMS
  → detectionRegions：ROI/定位坐标还原到相机图像
  → RuleRegistry：数量、置信度、文字等判定
```

## 新增 YOLOv8、SSD 等检测解码器

以 YOLOv8 为例，在 `src/modules/operators/det/decoders/yolov8/` 新建：

- `YoloV8Decoder.h`：继承 `DetectionDecoder`，声明 validate/decode 及模块定义函数。
- `YoloV8Decoder.cpp`：实现输出校验、解码、创建函数，以及 `yoloV8DecoderDefinition()`。

结构参考现有 `decoders/yolov5/YoloV5Decoder.h/.cpp`。
每种解码器自己声明 id、名称、创建函数和 ParameterSchema，不在 app 注册文件实现算法或参数表。

在 `src/app/registration/vision/RegisterDetectionDecoders.cpp` 加头文件和一条：

```cpp
yoloV8DecoderDefinition()
```

在 `src/Modules.cmake` 的 `hans_operators` 列表加入新的 cpp。
现有检测算子通过注册表生成模型格式选项与高级参数，参数保存为
`decoder.<id>.<参数名>`，切换模型格式时不同解码器的配置不会相互覆盖。
目前高级设置汇总所有已注册解码器的参数；只有所选解码器的配置会传入执行。
参数很多时应扩展通用条件显示能力，不为每个模型复制布局。

同类检测模型通常不需要修改公共 DetectionEngine、DetModule、工作台或方案读写。
只有输入语义/输出契约发生变化，才应扩展公共接口；例如双目或时序模型不是普通单图像检测适配。

## 解码器的输入输出约定

`TensorOutput::values` 是 RKNN 转换后的自持有 float32；即使模型属性为 INT8，也不要再次反量化。
float32 不代表经过 sigmoid，是否需要激活必须根据实际导出图确定。

`validate(outputs)` 检查张量 shape/layout 等是否符合当前适配器，不猜测其他 YOLO 版本。
`decode(outputs, context)` 返回 `DecodedDetections`：

- 每个目标填 classId、confidence、box；label 通常留空，由公共引擎映射字典。
- box 是模型输入图像坐标系中的像素矩形，不能提前还原成原图坐标。
- `context.expectedClasses` 非零时，校验模型类别数与字典是否一致。
- 需要公共 NMS 时返回 `Required`；模型已内置 NMS 时返回 `AlreadyApplied`。
- 没有目标返回空数组；不支持的格式或非法数据抛异常，不伪装成零目标。

## 修改模型与预处理

同输出格式的不同权重优先修改方案配置，不新建一份引擎。
YOLOv5 高级设置的自定义 anchors 是 `[3个尺度][3个anchor][宽,高]` 的 JSON，例如：

```json
[[[10,13],[16,30],[33,23]],[[30,61],[62,45],[59,119]],[[116,90],[156,198],[373,326]]]
```

顺序对应 stride 8/16/32。类别文件每行一个标签，行号从 0 开始对应 classId。
默认模型与字典位于 `resource/models/yolo/`；更换自训练模型时必须选择匹配字典。

公共 `DetectionOptions.preprocess` 支持颜色、缩放、padding 以及浮点归一化。
模型转换已内置的归一化不要在应用重复执行。多输入或新后端需要扩展公共层，不塞入 YOLOv5 解码文件。

## 新增其他视觉算子

边缘、颜色、尺寸测量等通常是独立算子，应在 `modules/operators/<名称>/` 定义自己的
OperatorDefinition，登记到 `app/registration/vision/RegisterOperators.cpp` 并加入 CMake。
声明 imageInput 即可复用图像/ROI 编辑，声明参数、输出、资源和规则即可复用相关界面及方案保存。
这些算法不需要为了使用注册机制而伪装成检测模型。

## 编译与检查

```bash
cd /tronlong/HansAiCameraApp
cmake --build build_rk3576 --target HansAiCameraApp -j4
cmake --build build_rk3576 --target test_yolov5_decoder test_detection_engine -j4
```

ARM64 测试程序不能直接在 x86 虚拟机执行。虚拟机原生构建目录为 build_host：

```bash
cmake -S . -B build_host -DBUILD_POSITION_CORRECTION_TEST=ON
cmake --build build_host --target test_yolov5_decoder test_detection_engine -j4
ctest --test-dir build_host -R '^test_(yolov5_decoder|detection_engine)$' --output-on-failure
```

解码器测试使用构造输出；引擎测试替换 RKNN 会话，执行真实的公共引擎、算子、判定与方案资源代码。
这些检查验证软件链路，不验证真实模型精度或 NPU 性能；板端模型与界面由用户验收。
