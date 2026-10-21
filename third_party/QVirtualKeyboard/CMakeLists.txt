# 第三方 QWidget 虚拟键盘(仿 Qt 官方 virtualkeyboard, 纯 Widgets 实现, Apache-2.0)
# 来源: https://gitee.com/third_party_project_code/QVirtualKeyboard
#
# 只构建业务实际需要的两块:
#   qtpinyin       - 拼音输入引擎, 纯 C++ 静态库
#   Qt5SoftKeyboard - QPlatformInputContext 插件(.so), 运行时需放入
#                     Qt 插件目录 plugins/platforminputcontexts/
# TestKeyboard 是原仓库自带的 demo, 不参与本项目构建

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Gui Widgets)

# ---------------- qtpinyin 静态库 ----------------
file(GLOB PINYIN_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/pinyin/share/*.cpp)

add_library(qtpinyin STATIC ${PINYIN_SOURCES})

target_include_directories(qtpinyin PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/pinyin/include
)

target_compile_definitions(qtpinyin PRIVATE
    HAVE_PINYIN
    QT_NO_CAST_TO_ASCII
    QT_ASCII_CAST_WARNINGS
    QT_NO_CAST_FROM_ASCII
    QT_NO_CAST_FROM_BYTEARRAY
)

target_link_libraries(qtpinyin PRIVATE Qt${QT_VERSION_MAJOR}::Core)

# ---------------- Qt5SoftKeyboard 插件 ----------------
add_library(Qt5SoftKeyboard MODULE
    qt5softkeyboard/inputcontext.cpp
    qt5softkeyboard/platforminputcontextplugin.cpp
    qt5softkeyboard/NumKeyboard.cpp
    qt5softkeyboard/NormalKeyboard.cpp
    qt5softkeyboard/pinyindecoderservice.cpp
    qt5softkeyboard/QAbstractInputMethod.cpp
    qt5softkeyboard/QPinyinInputMethod.cpp
    qt5softkeyboard/SymbolKeyboard.cpp
    qt5softkeyboard/KeyboardContainer.cpp
    qt5softkeyboard/QKeyboardLayout.cpp
    qt5softkeyboard/CandidatesListWidget.cpp
    qt5softkeyboard/NumKeyboard.ui
    qt5softkeyboard/NormalKeyboard.ui
    qt5softkeyboard/SymbolKeyboard.ui
    qt5softkeyboard/res.qrc
)

target_include_directories(Qt5SoftKeyboard PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/qt5softkeyboard
    ${CMAKE_CURRENT_SOURCE_DIR}/pinyin/include
)

target_compile_definitions(Qt5SoftKeyboard PRIVATE
    VIRTUALKEYBOARD_LIBRARY
)

target_link_libraries(Qt5SoftKeyboard PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::GuiPrivate
    Qt${QT_VERSION_MAJOR}::Widgets
    qtpinyin
)

# 输出到 <build>/plugins/platforminputcontexts, 与 Qt 插件目录结构一致
# 部署时把这个目录整体拷到目标机 Qt 安装的 plugins/ 下即可
set_target_properties(Qt5SoftKeyboard PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins/platforminputcontexts
    PREFIX ""
)
