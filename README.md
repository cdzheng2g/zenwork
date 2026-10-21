# HansAiCameraApp

RK3576 工业 AI 相机触控应用，使用 C++17、Qt Widgets、Rockit VI/VPSS、RKNN。

## 开发入口

- [目录职责与执行过程](docs/architecture.md)
- [新增算子、判断规则、通信方式](docs/extending.md)
- [编译和检查记录](docs/verification.md)
- [代码量统计](docs/code-counts.json)

在当前 Ubuntu 开发虚拟机编译：

```sh
cd /tronlong/HansAiCameraApp
sh scripts/build.sh
```

程序生成在 `build_rk3576/bin/HansAiCameraApp`。这是 ARM64 板端程序，不能直接在 x86 虚拟机运行。
当前脚本使用已有 Qt/sysroot 和 MPI SDK；换开发机时修改 toolchain 和 SDK 路径。

打包程序、Qt 运行库、平台插件和模型：

```sh
bash scripts/package_rk3576.sh
```

输出 `dist/HansAiCameraApp_rk3576.tar.gz`，在板端解压后通过 `./run.sh` 启动。
源码目录不保存用户的板端方案数据；实际方案目录由 `QStandardPaths::AppDataLocation` 确定。

统计自主维护的 C/C++ 源码，不包括第三方库、资源和构建产物：

```sh
python3 scripts/count_code.py
```

## 数据兼容

本地目录写入 formatVersion 2，便携 YAML 写入 formatVersion 3。
可读取旧本地目录版本 1、旧便携方案版本 1/2；旧 OCR 参数迁移到对应算子实例，原判定条件迁移为规则。
默认导出轻量方案，模型按 SHA-256 引用共享资源；完整迁移导出才携带模型。
新版本写出的方案不保证能由旧程序读取，因此原程序和原源码备份单独保留。
