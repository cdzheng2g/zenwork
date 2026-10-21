cmake_minimum_required(VERSION 3.12)

project(HansAiCamera VERSION 0.1 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE BOOL "Export compiler commands for editor tooling")

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 本机开发用系统 Qt5(apt安装);交叉编译到RK3576时通过
# -DQT_SDK_DIR=<tronlong-sysroot>/usr -DCMAKE_TOOLCHAIN_FILE=xxx.cmake 覆盖
set(QT_SDK_DIR /usr/lib/x86_64-linux-gnu CACHE PATH "QT SDK DIR")
list(APPEND CMAKE_PREFIX_PATH ${QT_SDK_DIR})

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Network Concurrent SerialBus)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network Concurrent SerialBus)

foreach (_qt_module Widgets Network Concurrent SerialBus)
    if (NOT "${Qt${QT_VERSION_MAJOR}${_qt_module}_VERSION}" VERSION_EQUAL "${Qt${QT_VERSION_MAJOR}Core_VERSION}")
        message(FATAL_ERROR
            "Qt ${_qt_module} (${Qt${QT_VERSION_MAJOR}${_qt_module}_VERSION}) must match Qt Core (${Qt${QT_VERSION_MAJOR}Core_VERSION}). Use modules from the same Qt installation.")
    endif ()
endforeach ()

# 交叉编译时moc/uic/rcc要用构建机(x86)上能跑的版本,而不是find_package从
# sysroot里解析出的aarch64版本(那份是给目标板子运行用的,不能在本机执行)
if (CMAKE_CROSSCOMPILING AND HOST_QT_BIN_DIR)
    foreach (_qt_tool moc uic rcc)
        if (TARGET Qt${QT_VERSION_MAJOR}::${_qt_tool})
            set_target_properties(Qt${QT_VERSION_MAJOR}::${_qt_tool} PROPERTIES
                IMPORTED_LOCATION "${HOST_QT_BIN_DIR}/${_qt_tool}")
        endif ()
    endforeach ()
endif ()

# 本机开发用系统apt装的OpenCV;交叉编译到RK3576时sysroot(如
# /tronlong/sys_ub/sysroots/armv8a-linux)下已自带同版本OpenCV 4.5.4的cmake配置,
# 通过CMAKE_FIND_ROOT_PATH/CMAKE_SYSROOT即可复用,不需要额外交叉编译OpenCV源码。
find_package(OpenCV REQUIRED COMPONENTS core imgproc imgcodecs)

if (NOT WIN32)
    add_link_options(-Wl,--disable-new-dtags)
endif ()

add_subdirectory(third_party/ElaWidgetTools)
add_subdirectory(third_party/QVirtualKeyboard)
set(YAML_CPP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(YAML_CPP_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
set(YAML_CPP_INSTALL OFF CACHE BOOL "" FORCE)
set(YAML_BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
add_subdirectory(third_party/yaml-cpp)
add_library(hans_yaml STATIC src/infrastructure/persistence/YamlDocument.cpp)
target_include_directories(hans_yaml PUBLIC ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(hans_yaml PUBLIC Qt${QT_VERSION_MAJOR}::Core yaml-cpp::yaml-cpp)
add_subdirectory(src)

option(BUILD_POSITION_CORRECTION_TEST "编译位置修正测试程序" OFF)
if (BUILD_POSITION_CORRECTION_TEST)
    enable_testing()
    add_subdirectory(tests)
endif ()
