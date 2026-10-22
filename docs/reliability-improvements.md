# 九项可靠性与架构改进

本文件描述实现边界；本次实际构建与测试结果见 verification.md。

| 原问题 | 当前实现 | 主要代码 / 验收 |
| --- | --- | --- |
| 1. 消费者仍存在但相机线程退出 | CameraRecovery 将需求、运行状态、有限重试分离；主线程在 worker finished 后重建采集链路，手动重连入口 | CameraStreamService、CameraRecovery；test_reliability |
| 2. 主线程同步存档 | 有条数及字节限制的 ArchiveQueue，写盘由 ArchiveThread 执行；失败带检测编号反馈 | ArchiveQueue、InspectionArchive、CreateInspectionService；test_reliability |
| 3. SDK 卡住导致无法退出 | 持久独立检测进程，本地套接字传值；父进程有截止时间和取消；终止子进程后锁定故障 | InspectionProcess、InspectionService；test_inspection_process |
| 4. TCP 缓冲无限增长 | 连接数、发送和接收上限，检查提交结果，慢客户端断开 | TcpServerService；test_tcp_limits |
| 5. 不同模型相互替换 | 稳定串行执行上下文，按模型路径缓存 RKNN 会话，LRU 淘汰、借用保护与文件大小预算 | ModelSessionCache、DetectionEngine、OcrEngine；test_model_migration |
| 6. 异步输出无法关联检测轮次 | 检测 UUID / 完整序号随 Modbus 请求传播，追踪提交、协议确认、失败、超时；事件异步存档 | ResultDeliveryTracker、ResultOutputDispatcher、ModbusTcpOutputService；test_reliability |
| 7. 工作台编排过多 | InspectionCoordinator 接收操作意图和模式状态；SchemeActivationService 应用方案，失败关闭准入；app 装配硬件服务 | InspectionCoordinator、SchemeActivationService、CreateInspectionCoordinator；test_inspection_coordinator |
| 8. 字段、版本缺少边界 | 帧 / 追踪 / 交付事件类型化；算子描述提供版本及迁移；检测缩放枚举持久化稳定键 | contracts、OperatorRegistry、ToolCodec、ParameterForm；test_model_migration |
| 9. 图像缺少编号和采集时间 | 帧信封随 mailbox、主线程、检测请求和 IPC 传递；按采集时间判断新鲜度并记录阶段耗时 | FrameEnvelope、InspectionTrace、InspectionService；test_inspection_coordinator / process |

## 读取调用链

1. main 创建应用；内部 worker 参数分支在 QApplication 与相机初始化前进入独立 QCoreApplication。
2. CameraStreamWorker 采集并标记帧编号 / 获取时刻；LatestFrameMailbox 只保留最新帧。
3. app 连接 CameraStreamService.frameAvailable 到 InspectionService.updateFrame。
4. 界面、内部定时器与 Modbus 统一进入 InspectionCoordinator.submit。
5. InspectionService 冻结图像 / 基准 / 方案、记录编号与代次，在专用单线程池调用 InspectionProcess。
6. 子进程解码请求，InspectionRunner 串行调用算子、规则与总判定，模型会话在该进程稳定复用。
7. 主线程 complete 判断代次、故障与生产序号，提交各输出及存档；磁盘操作在 ArchiveThread。
8. Modbus 回复与存档结果回到主线程，ResultDeliveryTracker 按检测编号更新并异步记录事件。

## 不能混淆的状态

- 有采集消费者，不代表相机正在出帧。
- 检测算法 OK，不代表每个输出通道交付成功。
- socket.write 接受数据，不代表对方已收到。
- Modbus 成功响应，不代表 PLC 完成业务动作。
- 定时器触发，不等于可以安全杀掉线程；应用通过独立进程划定故障边界。

## 保留的限制

- 算子可扩展 parameters / fields 仍是受元数据校验的 JSON；本次没有把所有扩展字段硬编码成 C++ 结构体。公共运行契约已类型化。
- 采集使用已有 AIQ / VI / VPSS 主线程初始化约束；硬件重启成功率、设备资源释放、SDK 超时行为须板端验收。
- 相机 worker 的正常停止依赖有超时的取帧 API；采集 SDK 完全挂死的设备级处理不由推理隔离解决。
- 最多 4 个模型会话时，超过缓存容量的方案仍可能逐轮加载；应按目标板内存和实际模型组合测量。
- 存档队列满时拒绝；退出等待超时可能失去未写记录。没有宣称断电耐久或恰好一次交付。
- 推理被隔离后图像经 PNG / JSON 传输，有复制和编码开销。当前串行方案应实测总时延后设置限时。
