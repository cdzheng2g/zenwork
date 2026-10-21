# VS Code SSH 开发配置

在 SSH 窗口打开 `/tronlong/HansAiCameraApp` 根目录。
Windows 本机安装的 C/C++ 扩展不会代替 SSH 主机上的语言服务；远端现已安装 `ms-vscode.cpptools`。

## 补全来源

- `.vscode/c_cpp_properties.json` 指定 ARM64 交叉编译器、sysroot、C++17 和编译命令数据库。
- `build_rk3576/compile_commands.json` 由 CMake 生成，记录已加入构建的每个源文件的真实编译参数。
- 对尚未加入 CMake 的学习草稿，提供 src、Qt、OpenCV、RKNN 的回退头文件路径。
- 头文件按 C++ 识别，启用成员补全、参数提示和错误提示。

## 使用

安装完成后，如当前窗口未自动激活扩展，按 Ctrl+Shift+P，执行 `Developer: Reload Window`（开发人员：重新加载窗口）。
第一次索引需要等待。打开 cpp 文件后，输入 `options.`、`context.`，或按 Ctrl+Space 查看建议。
F12 跳转定义；Ctrl+Shift+Space 显示参数提示；Ctrl+Shift+B 编译板端程序。

## 新增文件后

新 cpp 加入 `src/Modules.cmake` 后，运行任务“CMake: 更新补全数据库”，或执行：

```bash
cmake -S . -B build_rk3576 -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
```

`scripts/build.sh` 已启用数据库导出。不要手工修改 compile_commands.json。
首次在全新目录编译时用 `sh scripts/build.sh` 初始化交叉编译配置。
本配置针对当前 Ubuntu SDK 路径；换开发机时需同步修改 toolchain 和编辑器回退路径。
