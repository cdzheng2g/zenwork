# RK3576(aarch64)交叉编译工具链。用法:
#   cmake -B build_rk3576 -DCMAKE_TOOLCHAIN_FILE=cmake/toolchain_aarch64_rk3576.cmake \
#         -DQT_SDK_DIR=/tronlong/sys_ub/sysroots/armv8a-linux/usr
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)

set(CMAKE_C_COMPILER /usr/bin/aarch64-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER /usr/bin/aarch64-linux-gnu-g++)
set(CMAKE_STRIP /usr/bin/aarch64-linux-gnu-strip)

# 板子sysroot:头文件/库都从这里找,而不是本机x86的/usr
set(CMAKE_SYSROOT /tronlong/sys_ub/sysroots/armv8a-linux)
set(CMAKE_FIND_ROOT_PATH /tronlong/sys_ub/sysroots/armv8a-linux)

# 只在sysroot里找库/头文件/cmake包,可执行程序(如protoc之类的host工具)仍用本机的
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# 链接时把rpath指向sysroot里的库目录,避免运行时找不到.so(板子上部署路径一致时也适用)
set(CMAKE_INSTALL_RPATH /usr/lib/aarch64-linux-gnu)
set(CMAKE_BUILD_WITH_INSTALL_RPATH ON)

# moc/uic/rcc是代码生成工具,运行在构建机(x86)上,不是目标程序,不能用sysroot里的
# aarch64版本。Qt5的cmake配置里这几个imported target的路径是硬编码到sysroot下的,
# 交给顶层CMakeLists.txt在find_package(Qt5...)之后覆盖成本机路径(本机Qt同为5.15.3)。
set(HOST_QT_BIN_DIR /usr/lib/x86_64-linux-gnu/qt5/bin CACHE PATH "")


