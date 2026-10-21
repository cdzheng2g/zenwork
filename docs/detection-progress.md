# 目标检测当前实现（2026-10-20）

## 代码位置

| 位置 | 职责 |
| --- | --- |
| `src/application/vision/detection/DetectionEngine.h/.cpp` | 图像预处理、RKNN 调用、解码器选择、标签映射、坐标还原和 NMS |
| `src/application/vision/detection/DetectionDecoder.h` | 统一解码接口，输出模型输入坐标系中的候选框 |
| `src/application/vision/detection/DetectionDecoderRegistry.h/.cpp` | 查找/创建解码器，汇总各解码器的参数元数据 |
| `src/application/vision/detection/DetectionOptions.h` | 模型路径、标签和预处理配置 |
| `src/modules/operators/det/DetModule.h/.cpp` | 检测算子的参数、资源、执行入口、输出和默认规则 |
| `src/modules/operators/det/decoders/yolov5/YoloV5Decoder.h/.cpp` | YOLOv5 解码、创建函数与专属参数声明 |
| `src/app/registration/vision/RegisterDetectionDecoders.cpp` | 选择应用启用的解码器，仅保留注册清单 |
| `src/app/registration/vision/RegisterOperators.cpp` | 启用目标检测及其他视觉算子 |

旧 `DetEngine.h/.cpp` 已迁移到公共引擎并删除。
旧的未完成检测示例及其编译目标已删除，避免与当前公共引擎、教程产生冲突。
通信、规则与界面注册分别移到 `app/registration/communication`、`rules`、`ui`。

## 应用中的配置与结果

在方案的算子库中添加“目标检测”，复用通用图像编辑器，不编写独立检测页面。

- 默认整图检测；可以设置 ROI，也可选择使用上游定位校正图。
- 默认模型：`resource/models/yolo/yolov5s-640-640.rknn`。
- 默认类别：`resource/models/yolo/coco_80_labels_list.txt`，每行对应一个从 0 开始的类别。
- 普通参数包含模型格式、模型/标签路径、置信度、NMS、缩放和颜色转换。
- 高级参数包含 YOLOv5 的 sigmoid 开关和自定义 anchors JSON；空 anchors 使用默认三组。
- 输出 `count`、`confidence`（最高值，没有目标时为 0）、`labels` 和 `regions`。
- 默认规则为目标数量恰好等于 1。可以在判定区调整数量、置信度或类别文字规则。
- 没有目标是有效的零目标结果，默认规则给 NG；模型/格式/字典错误给 ERROR。
- 图像框沿用通用 polygon/regions 通道，显示类别和置信度，经过 ROI 与定位变换还原到相机原图。
- 模型与标签都已登记资源元数据，沿用轻量 YAML / 完整方案的共享或内嵌资源机制。

## 支持边界

当前 YOLOv5 适配器支持三个 NCHW 原始检测头，batch=1，stride 8/16/32，每尺度 3 个 anchors。
不接受已经合并解码的 `[1,N,85]` 输出、YOLOv8 DFL 或内置 NMS 的不同输出格式。
模型类别数必须与标签文件行数一致；输出顺序可以变化，通过 stride 选择 anchors。

公共 RKNN 会话返回已反量化的 float32；这不表示已执行 sigmoid。
`outputsAreLogits` 默认 false，与当前教程的已 sigmoid 输出约定一致。
真实模型必须核对颜色、归一化、补边、anchors 和输出格式，不能仅凭文件名推断兼容。

公共引擎支持单图像、batch=1、固定三通道 NCHW/NHWC 模型输入。
可提交 UINT8、FLOAT32、FLOAT16 图像；浮点输入按 `(pixel * scale - mean) / standardDeviation` 处理。
UINT8 图像的归一化/量化由模型的 RKNN 转换配置处理，不在应用中重复处理。
不直接构造 INT8 图像输入；量化模型可接收 UINT8 图像，由 RKNN 完成转换。
模型会话与解码器在检测线程内复用，模型专属配置改变时重建解码器。

## 验证

RK3576 主程序及检测测试目标交叉编译通过。
Ubuntu 原生构建的 test_yolov5_decoder、test_detection_engine、test_module_contracts、test_operator_editors 四组测试全部通过，共 70 项断言。
覆盖三尺度解码、anchors、sigmoid、NMS、RGB/BGR、原图不被修改、FLOAT32/FLOAT16、Letterbox/Stretch、ROI 画框坐标、字典不匹配、无目标 NG、错误 ERROR、元数据界面和完整 YAML 资源往返。
日志见 `docs/checks/detection-20261020/`。
虚拟机算法测试使用可控推理输出，不能代替板端真实模型和界面验收，也不代表已测得真实 NPU 耗时。
本次不部署、不启动板端应用。

为运行 Ubuntu 原生测试，安装了本机 `libqt5serialbus5-dev` 及其依赖，没有升级其他软件包。

## 代码规模

统计 src/tests 下 C/C++ 文件物理行数，含空行和注释，不含第三方库、文档和构建产物。

| 范围 | 修改前 | 修改后 |
| --- | --- | --- |
| src | 167 文件 / 12,360 行 | 168 文件 / 12,451 行 |
| tests | 16 文件 / 1,516 行 | 17 文件 / 1,732 行 |

主源码净增 91 行，包含公共引擎、解码器注册、检测算子接入和旧草稿清理。

## 修改前备份

虚拟机：`/tronlong/HansAiCameraApp/backups/detection-integration-20261020-173800/source.tar.gz`。
包含修改前 src、tests、docs、CMakeLists.txt 和 cmake 目录。
