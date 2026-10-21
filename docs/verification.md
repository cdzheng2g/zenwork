# 编译和检查记录

日期：2026-10-16。

## 产物与原项目

- 新项目：Ubuntu `/tronlong/HansAiCameraApp`。
- 原项目：`/tronlong/hans_ai_camara`；与开始时的源码备份逐文件 SHA-256 比较，未发现修改。
- 源码备份：`/tronlong/HansAiCameraApp-source-backup-20261016.tar.gz`。
- ARM64 程序：`build_rk3576/bin/HansAiCameraApp`。
- 独立部署包：`dist/HansAiCameraApp_rk3576.tar.gz`，约 128 MiB，包含运行库、插件与资源。
- 原板端安装未替换；本轮没有启动新主程序或进行实际界面交互测试。

编译和打包完成。可执行文件与部署包 SHA-256 在 [checks/SHA256SUMS](checks/SHA256SUMS)。
完整日志：[编译](checks/build.log)、[打包](checks/package.log)。

## 板端非交互检查

测试程序放在 `/home/zen/HansAiCameraApp_checks`，使用独立数据目录和临时网络端口。
UI 数据检查采用 Qt minimal 后端，不显示到正在使用的桌面，也不控制相机采集。

12 个测试程序退出码均为 0：

| 程序 | 验证内容 |
| --- | --- |
| test_module_contracts | 同类算子独立参数、规则、旧配置迁移、非法配置拒绝、共同触发准入 |
| test_operator_editors | 实际编辑页的两个 OCR 实例隔离、ROI/模型保存、切换/删除实例、规则开关、数字控件联动 |
| test_inspection_service | 后台执行、帧快照、忙碌拒绝、切换方案使旧结果失效、单算子测试不发送生产结果 |
| test_scheme_catalog | 方案目录读写与方案操作 |
| test_scheme_package | 轻量/完整 YAML、模型指纹、跨目录资源恢复、通信设置、旧版本迁移、错误资源拒绝 |
| test_modbus_server | 保持寄存器读写、触发反馈、结果映射、服务重启 |
| test_scheme_communication | 自动启停、连接保留、监听端口切换、冲突拒绝、Client 写入 |
| test_inspection | 模板执行、框坐标、空图/无基准错误、旧非 UUID 标识的结果隔离 |
| test_position_correction | 合成旋转/平移图像的位置修正 |
| test_camera_stream_lease | 相机采集引用管理 |
| test_reference_import | 空图及非法名称处理 |
| test_file_operations | 最新帧邮箱不会堆积过时帧 |

额外使用板上现有 RKNN 模型运行 `test_inspection <模型目录>`，真实验证区域 OCR、整图检测加识别、
倒置文字、错误文本判 NG、缺少模型判 ERROR。输入为生成的 `ABC123` 图像，不依赖用户场景。

以上执行共通过 150 条检查，日志保存在 `checks/`。结果索引见 [results.tsv](checks/results.tsv)。
`test_template_performance` 作为需要图像与区域文件参数的手动基准程序编译，不列入无参数自动检查。
长时间连续生产、现场光照识别效果、触屏布局和实际操作流程仍需由用户验收。

## 代码行数

统计 `src/` 与 `tests/` 中 `.h/.hpp/.cpp/.cc/.c`，排除第三方库、资源、构建目录和备份副本。
总行数包含空行和注释；非空行数仍包含注释，不应与纯逻辑语句数量混淆。
旧项目的 CRCRLF 先归一为单个换行；新代码统一格式化，因此行数变化不单独用来衡量去重效果。

| 范围 | 原总行数 | 新总行数 | 原非空行 | 新非空行 |
| --- | ---: | ---: | ---: | ---: |
| 业务源码 src | 11,920 | 11,263 | 10,497 | 10,267 |
| 测试 tests | 815 | 1,147 | 751 | 1,100 |
| 合计 | 12,735 | 12,410 | 11,248 | 11,367 |

业务源码减少 657 总行、230 非空行。新验证增加 332 总行。
源码文件数从 131 到 142：职责拆开后文件数增加，原有无调用的执行链、测试页面和重复输出实现已移除。
移除清单见 `removed-files.json`；搬迁对应关系见 `source-map.json`。

复算：

```sh
python3 scripts/count_code.py \
  --baseline /tronlong/HansAiCameraApp-source-backup-20261016.tar.gz \
  --output docs/code-counts.json
```
