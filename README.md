# HansAiCameraApp

面向 RK3576 工业 AI 相机的 C++17 / Qt Widgets 应用。支持相机预览、方案管理、模板定位、OCR、目标检测、灰度统计、规则判定，以及 TCP / Modbus 结果输出。

## 阅读入口

- [架构与进程、线程职责](docs/architecture.md)
- [九项改进的实现与验收](docs/reliability-improvements.md)
- [新增算子、规则和通信方式](docs/extending.md)
- [目标检测模型接入](docs/adding-detection.md)
- [验证记录](docs/verification.md)

## 一次检测怎么运行

```text
应用进程
  主线程：界面、方案激活、触发准入、Qt 网络事件、结果展示
      ↑ 最新帧（编号、采集时间）
  采集线程：获取帧、转换图像、只保留最新帧
      ↓ 图像与方案快照
  固定执行线程：与独立检测进程通信、监测超时和取消
      ↓ 本地套接字
独立检测进程
  串行执行算子 → 规则判定 → 汇总结果
  RKNN 模型会话缓存：按模型路径复用，限制数量和模型文件总大小

应用主线程收到结果
  → 检查方案代次，丢弃过期结果
  → 提交网络输出，按检测编号追踪交付状态
  → 存档队列 → 独立写盘线程
```

程序通过自身的内部入口 `--inspection-worker` 启动检测子进程，发行包只需要同一个应用可执行文件。RKNN SDK 可能创建自己的内部线程；相机驱动与 SDK 的线程不由上图枚举。

## 分层

| 目录 | 职责 |
| --- | --- |
| `src/contracts` | 算子、触发、帧、追踪与交付事件等共享数据契约 |
| `src/domain` | 判断状态及 AND / OR 合并规则 |
| `src/application` | 检测协调、方案激活、执行和交付追踪 |
| `src/modules` | 算子、判断规则、通信模块 |
| `src/infrastructure` | 相机、RKNN、子进程、网络和持久化实现 |
| `src/ui` | 页面、参数输入、操作意图与结果展示 |
| `src/app` | 模块注册和具体服务装配 |

模块在编译时注册。方案激活和触发准入可在没有 QWidget 的测试中运行。

## Ubuntu 本机开发

需要 CMake、C++17 编译器、Qt（Core / Widgets / Network / Concurrent / SerialBus，同一版本）、OpenCV（core / imgproc / imgcodecs）。仓库包含所用第三方 UI 库和 yaml-cpp。

```sh
sudo apt install build-essential cmake qtbase5-dev libqt5serialbus5-dev libopencv-dev
cmake -S . -B build_host -DBUILD_POSITION_CORRECTION_TEST=ON
cmake --build build_host -j2
QT_QPA_PLATFORM=offscreen ctest --test-dir build_host --output-on-failure --timeout 45
```

本机可调试界面、方案和算法公共逻辑；没有 RKNN / AIQ / Rockit 时相关硬件能力不可用。测试中的 SDK 替身和本机网络测试不能代替板端验收。

## RK3576 交叉编译

当前云端开发环境已配置 ARM64 工具链和板端 sysroot：

```sh
ROCKIT_MPI_SDK_DIR=/path/to/mpi/sdk JOBS=2 sh scripts/build.sh
```

脚本默认使用：

- sysroot：`/tronlong/sys_ub/sysroots/armv8a-linux`
- 主机 Qt 工具：`/tronlong/sys_ub/sysroots/x86_64-linux/bin`
- Rockit SDK：`/home/zen/下载/rk_mpi/mpi/sdk`

换机器需要调整 `cmake/toolchain_aarch64_rk3576.cmake` 和脚本路径；AIQ 运行库与板端 IQ 文件须配套。输出 `build_rk3576/bin/HansAiCameraApp` 是 ARM64 程序，不能在 x86 云服务器运行。

打包：

```sh
bash scripts/package_rk3576.sh
```

输出 `dist/HansAiCameraApp_rk3576.tar.gz`，板端解压后执行 `./run.sh`。打包不等于部署或硬件验收。

## 运行边界与故障处理

| 项目 | 当前默认值 / 行为 |
| --- | --- |
| 相机恢复 | 保留消费者需求；意外退出后最多自动重试 3 次，退避 1 / 2 / 4 秒；失败后可在相机页手动重连 |
| RKNN 单次运行超时 | 30 秒；`HANS_RKNN_RUN_TIMEOUT_MS` 可设 1–600000 毫秒 |
| 整轮检测超时 | 60 秒；`HANS_INSPECTION_TIMEOUT_MS` 可设 1–600000 毫秒 |
| 检测故障 | 停止接收新检测，终止子进程；保留故障状态，需重启应用恢复 |
| 模型缓存 | 每执行线程最多 4 个会话，模型文件合计 256 MiB，LRU 淘汰空闲会话 |
| 存档队列 | 最多 256 条、8 MiB（含正在写入项）；队列满时拒绝并报告，不阻塞界面等待空间 |
| 存档退出等待 | 最多 2 秒；未排空会报告，进程退出可能丢失未完成记录 |
| TCP | 最多 16 客户端；每客户端待发送缓冲 1 MiB；拒绝输入应用数据 |
| 交付追踪 | 最多保留 256 条通道状态；待确认 5 秒超时 |

正常总耗时较长的多模型、多文字区域方案，需要按实测调整检测超时。模型文件预算不包含 SDK 工作区，NPU 内存占用仍须在板端测量。常规退出会取消独立检测进程；操作系统中的不可终止设备调用仍需设备级恢复。

TCP 成功提交和 Modbus 服务端寄存器更新记为 `unconfirmed`；Modbus 客户端收到协议成功响应记为 `confirmed`，不表示 PLC 业务已经消费结果。若产线需要重试、去重或业务握手，需要双方约定协议。

## 数据与兼容

- 图像和方案在触发时冻结；切换方案后的旧检测结果不进入生产输出。
- 每轮有 UUID 检测编号，序号和帧编号以字符串保存，避免 JSON 大整数精度损失。
- 帧时间来自主机成功获取帧的单调时钟，尚非传感器曝光时间。结果包含触发时帧龄、队列等待和完成耗时。
- 检测结果与交付事件分别写入 JSON 行日志，使用检测编号关联。
- 本地方案目录 formatVersion 2，便携 YAML formatVersion 3；可读旧本地版本 1、旧便携版本 1 / 2。
- 算子按自身版本迁移；目标检测 v1 的中文缩放方式迁移为 v2 的 `letterbox / stretch`，界面继续显示中文。
- 模型按 SHA-256 引用共享资源；完整迁移导出才携带模型。
- 新版本保存后不保证旧程序可读，升级前保留原始方案备份。

实际方案和日志目录由 `QStandardPaths::AppDataLocation` 确定，不属于源码仓库。

## Git 与生成文件

`build_host/`、`build_rk3576/`、`dist/` 等生成目录已纳入忽略规则。编译产物留在构建机器，不提交 Git。历史提交中的旧构建文件仍属于 Git 历史，本次不重写历史。

统计自主维护的代码：

```sh
python3 scripts/count_code.py
```
