# 目录职责与执行过程

## 从哪里读代码

| 位置 | 负责什么 | 不应该放什么 |
| --- | --- | --- |
| `src/contracts/` | 算子实例、参数描述、规则描述、通信描述、触发准入和基准图等公共契约 | QWidget、网络连接实现、具体 OCR 推理 |
| `src/domain/project/` | OK/NG/ERROR/SKIPPED 状态以及 AND/OR 求值 | 文件保存、控件更新 |
| `src/application/` | 检测调度、执行算子、方案读写与迁移、通信通道调度 | 绘制控件、按算子类型堆积分支 |
| `src/modules/operators/` | 各算子的描述、算法实现和可选专用编辑器 | 整个工作台的控制逻辑 |
| `src/modules/rules/` | 独立判断规则和注册表查询 | 模型推理、相机驱动 |
| `src/modules/communication/` | 各协议的参数、状态、启动和结果映射 | 方案编辑页布局 |
| `src/infrastructure/` | 相机、网络、存储、系统统计、账号等具体实现 | 判断规则的业务含义 |
| `src/ui/` | 页面、通用控件、参数编辑器和检测结果显示 | OCR 推理、协议打包 |
| `src/app/` | 集中注册模块和连接应用服务的具体实现 | 重复实现模块业务 |

入口是 `main.cpp`，登录后创建 `ui/MainWindow` 和 `ui/workbench/WorkbenchWidget`。
要了解新增功能，先读 `app/registration/vision/RegisterOperators.cpp`、`app/registration/rules/RegisterRules.cpp`、`app/registration/communication/RegisterCommunication.cpp`。
注册清单按 vision、communication、rules、ui 分组。具体模块自行提供定义与创建函数，app 不实现算法或参数业务。
原 `core/` 中混放的服务已归入 `infrastructure/camera`、`communication`、`reference`、`auth`、`system`、`input`。
原模板算法实现已归入 `modules/operators/template_match`；旧位置对照见 `source-map.json`。

## 一次检测

```text
CameraStreamService ──最新帧──> InspectionService
                                   ↑
内部定时 / 手动按钮 / Modbus ──共同准入规则
                                   ↓
                      冻结本轮方案、图像与基准图
                                   ↓ 后台线程
                         InspectionRunner
                                   ↓
                OperatorRegistry → 算子输出 fields
                                   ↓
                    RuleRegistry → 各判断结果
                                   ↓
                       LogicEvaluator → 总判定
                                   ↓ 主线程
        当前方案仍有效 → 结果输出、记录、界面显示
```

检测期间不排队堆积新任务；正在执行时返回 Busy。切换方案或取消有效运行上下文会使旧任务结果失效。
单算子测试包含其前方必要定位，但不增加生产序号、不发送生产结果、不写生产记录。
预览持续采集最新帧，推理使用独立图像快照；视觉验收需要在实际触屏或远程 Qt 界面进行。

`InspectionService` 注入执行函数和结果接收函数。测试可以提供假执行器；实际程序由
`app/CreateInspectionService.cpp` 接入真实算法、通信和结果存档。

## 算子、规则、界面如何联系

每个 `SchemeTool` 有独立 `id / kind / version / parameters / rules`。
两个 OCR 的区域、模型和判定条件分别保存，不再共用全局 OCR 参数。

`OperatorDefinition` 声明参数、输出字段、资源、执行函数、默认规则和自定义校验函数。
算子只负责计算输出。文字相等、长度、非空、数值范围由独立规则求值。
同一算子的启用规则全部通过才是 OK；算法异常或规则输入缺失是 ERROR。
方案层再按 AND/OR 合并，ERROR 不会被其他算子的 OK 覆盖。

`ParameterForm` 根据参数描述生成数字框、滑动条、开关、文本框、下拉框和文件选择框。
`AppTheme` 和 `NumericParameterField` 集中维护公共样式与数值联动；相机页复用数值组件。
`OperatorEditorRegistry` 对普通算子使用通用表单；声明 `imageInput` 的算子自动使用通用 `ImageOperatorEditor`。
OCR 已复用通用编辑器；模板定位仍保留专用编辑器。文件参数按显式 `advanced` 属性折叠，不再根据文本类型猜测是否为模型。
通用结果展示使用模块声明的输出字段；图像框使用现有 polygon/regions 结果结构。

## 图像与 RKNN 的公共能力

- `application/vision/ImageInput`：原图/校正图选择、ROI 校验与裁剪、ROI 到相机原图的坐标映射。
- `application/vision/ImageTransform`：Stretch / Letterbox，保存实际缩放和补边的逆变换。
- `application/vision/DetectionResults`：置信度过滤、同类别 NMS、生成通用检测框数据。
- `application/vision/detection/DetectionEngine`：公共图像预处理、RKNN 推理、解码器选择、标签与坐标还原。
- `application/vision/detection/DetectionDecoderRegistry`：解码器查询/创建与模型专属参数元数据。
- `infrastructure/inference/rknn/RknnSession`：模型生命周期、属性查询、多输入/多输出、float32 输出复制和 SDK 缓冲区释放。
- `application/operators/OperatorResources`：指定资源路径和打包默认路径解析。

OCR 仅保留自身的文字检测、识别预处理、字典和 CTC 解码，不再直接调用 RKNN SDK 或实现独立 ROI 页面。
模型会话在工作线程内复用，失败的新模型初始化不会破坏原会话。
新模型的前处理约定和输出解码仍由模型适配器负责；通用会话没有假设所有检测模型都与 OCR 一样。
详细扩展教程和当前代码入口见 [adding-detection.md](adding-detection.md)。

## 构建约束

`src/Modules.cmake` 分别构建契约、规则、推理、算子、协议、方案、运行调度、基准存储、通用参数 UI 和算子 UI。
运行调度、协议和判断规则不链接项目的 QWidget 模块。OpenCV 只使用 core、imgproc、imgcodecs。
专用图像编辑器虽然与算子放在同一目录，但属于单独 UI 构建目标，不进入后台运行库。

这是编译时注册的模块化单体，不是运行时加载 `.so` 的插件系统。增加模块后仍需重新编译。
当前仍为串行检测；多 OCR 实例已支持，模板定位保留一个共享坐标系，因此一个方案只允许一个定位算子。
本轮没有增加流程图、并行图调度或多定位坐标绑定。
