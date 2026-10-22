cmake_minimum_required(VERSION 3.12)

project(HansAiCameraApp VERSION 0.1 LANGUAGES CXX)

set(PROJECT_SOURCES
    main.cpp
    application/operators/OperatorExecution.h
    application/scheme/CommunicationSettings.h
    infrastructure/communication/LegacyCommunicationSettings.h
    infrastructure/communication/ModbusRegisterMap.h
    ui/LoginWindow.h
    ui/LoginWindow.cpp
    ui/ImageCanvasWidget.h
    ui/ImageCanvasWidget.cpp
    ui/pages/ReferenceManagePage.h
    ui/pages/ReferenceManagePage.cpp
    ui/pages/CameraSettingsPage.h
    ui/pages/CameraSettingsPage.cpp
    ui/pages/CommunicationSettingsPage.h
    ui/pages/CommunicationSettingsPage.cpp
    ui/communication/CommunicationPanelRegistry.h
    ui/communication/CommunicationPanelRegistry.cpp
    ui/pages/AlgorithmConfigPage.h
    ui/pages/AlgorithmConfigPage.cpp
    ui/MainWindow.h
    ui/MainWindow.cpp
    ui/workbench/WorkbenchWidget.h
    ui/workbench/WorkbenchWidget.cpp
    ui/workbench/layout/WorkbenchLayout.cpp
    ui/workbench/scheme/WorkbenchSchemes.cpp
    ui/workbench/inspection/WorkbenchInspection.cpp
    ui/workbench/files/WorkbenchFiles.cpp
    ui/workbench/LiveMonitorWidget.h
    ui/workbench/LiveMonitorWidget.cpp
    ui/workbench/ImageInputPanel.h
    ui/workbench/ImageInputPanel.cpp
    infrastructure/auth/AccountManager.h
    infrastructure/auth/AccountManager.cpp
    infrastructure/reference/ReferenceImageManager.h
    infrastructure/camera/CameraParams.h
    infrastructure/system/SystemResourceMonitor.h
    infrastructure/system/SystemResourceMonitor.cpp
    infrastructure/camera/CameraController.h
    infrastructure/camera/CameraController.cpp
    infrastructure/camera/CameraStreamWorker.h
    infrastructure/camera/CameraStreamWorker.cpp
    infrastructure/camera/CameraStreamLease.h
    infrastructure/camera/CameraStreamLease.cpp
    infrastructure/camera/CameraStreamService.h
    infrastructure/camera/CameraStreamService.cpp
    infrastructure/camera/CameraParamsStore.h
    infrastructure/camera/CameraParamsStore.cpp
    infrastructure/input/ImageSequenceSource.h
    infrastructure/input/ImageSequenceSource.cpp
    contracts/ReferenceImage.h
    app/CreateInspectionService.cpp
    app/CreateInspectionCoordinator.cpp
)

include(${CMAKE_CURRENT_SOURCE_DIR}/Modules.cmake)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ui
    ui/pages
    ui/workbench
    modules/operators/ocr
    ${CMAKE_SOURCE_DIR}/third_party/rknn/include
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Concurrent
    Qt${QT_VERSION_MAJOR}::Network
    Qt${QT_VERSION_MAJOR}::SerialBus
    hans_runtime hans_schemes hans_operator_editors
    Ela::WidgetTools
    ${OpenCV_LIBS}
)

# RKNN运行库只在RK3576板端可用;头文件随项目第三方目录提供,
# 交叉编译时链接sysroot中的librknnrt.so。
find_library(RKNN_RT_LIBRARY rknnrt)
if (RKNN_RT_LIBRARY)
    target_compile_definitions(${PROJECT_NAME} PRIVATE HAVE_RKNN)
    target_compile_definitions(hans_inference PRIVATE HAVE_RKNN)
    target_link_libraries(hans_inference PRIVATE ${RKNN_RT_LIBRARY})
    target_link_libraries(${PROJECT_NAME} PRIVATE ${RKNN_RT_LIBRARY})
else ()
    message(STATUS "librknnrt未找到,OCR 推理不可用")
endif ()

# 相机成像设置(CameraController)对接RK3576 rkaiq ISP API;交叉编译到板端时
# sysroot下自带librkaiq及其uAPI2头文件,本机x86开发环境没有,find_library/
# find_path会找不到,此时CameraController.cpp退化为纯内存态的空实现,
# 不影响UI/参数持久化在本机的开发调试。
# The SC130GS board IQ requires the vendor-paired AIQ runtime.
if (CMAKE_CROSSCOMPILING AND EXISTS "${CMAKE_SOURCE_DIR}/third_party/rkaiq/lib/librkaiq.so")
    set(RKAIQ_LIBRARY "${CMAKE_SOURCE_DIR}/third_party/rkaiq/lib/librkaiq.so" CACHE FILEPATH "Board-paired AIQ runtime" FORCE)
else ()
    find_library(RKAIQ_LIBRARY rkaiq)
endif ()
find_path(RKAIQ_INCLUDE_DIR uAPI2/rk_aiq_user_api2_ae.h PATH_SUFFIXES rkaiq)

if (RKAIQ_LIBRARY AND RKAIQ_INCLUDE_DIR)
    target_compile_definitions(${PROJECT_NAME} PRIVATE HAVE_RKAIQ)
    target_include_directories(${PROJECT_NAME} PRIVATE
        ${RKAIQ_INCLUDE_DIR}
        ${RKAIQ_INCLUDE_DIR}/uAPI2
        ${RKAIQ_INCLUDE_DIR}/algos
        ${RKAIQ_INCLUDE_DIR}/common
        ${RKAIQ_INCLUDE_DIR}/xcore
    )
    target_link_libraries(${PROJECT_NAME} PRIVATE ${RKAIQ_LIBRARY})
else ()
    message(STATUS "librkaiq未找到,相机成像设置功能将以空实现编译(仅本机调试用)")
endif ()

# VI->VPSS->VO硬件叠加预览对接RK3576 rockit MPI SDK。该SDK目前只在开发机本地
# 路径下存在(非sysroot、未纳入版本控制),交叉编译到板端时通过
# -DROCKIT_MPI_SDK_DIR=<path> 指定;本机x86开发环境不设置该变量时,
# CameraController.cpp的VI/VPSS/VO部分退化为空实现。
set(ROCKIT_MPI_SDK_DIR "" CACHE PATH "Rockit MPI SDK目录(需包含include/和lib/lib64/librockit.so)")

if (RKAIQ_LIBRARY AND RKAIQ_INCLUDE_DIR AND ROCKIT_MPI_SDK_DIR)
    # 交叉编译时CMAKE_FIND_ROOT_PATH_MODE_LIBRARY/INCLUDE为ONLY,find_library/find_path
    # 默认只在sysroot下找;rockit SDK是sysroot外的机器本地路径,需要NO_CMAKE_FIND_ROOT_PATH
    # 显式跳过这一限制(单纯NO_DEFAULT_PATH不够,那个只关闭标准系统路径,不影响根路径前缀逻辑)。
    find_library(ROCKIT_LIBRARY rockit
        PATHS ${ROCKIT_MPI_SDK_DIR}/lib/lib64
        NO_DEFAULT_PATH
        NO_CMAKE_FIND_ROOT_PATH
    )
    find_path(ROCKIT_INCLUDE_DIR rk_mpi_sys.h
        PATHS ${ROCKIT_MPI_SDK_DIR}/include
        NO_DEFAULT_PATH
        NO_CMAKE_FIND_ROOT_PATH
    )

    if (ROCKIT_LIBRARY AND ROCKIT_INCLUDE_DIR)
        target_compile_definitions(${PROJECT_NAME} PRIVATE HAVE_ROCKIT_MPI)
        target_include_directories(${PROJECT_NAME} PRIVATE
            ${ROCKIT_INCLUDE_DIR}
            ${ROCKIT_MPI_SDK_DIR}/lib/lib64
        )
        target_link_directories(${PROJECT_NAME} PRIVATE ${ROCKIT_MPI_SDK_DIR}/lib/lib64)
        target_link_libraries(${PROJECT_NAME} PRIVATE ${ROCKIT_LIBRARY} dl)

        add_executable(CamVoViProbe tools/CamVoViProbe.cpp)
        target_compile_definitions(CamVoViProbe PRIVATE HAVE_RKAIQ HAVE_ROCKIT_MPI)
        target_include_directories(CamVoViProbe PRIVATE
            ${RKAIQ_INCLUDE_DIR}
            ${RKAIQ_INCLUDE_DIR}/uAPI2
            ${RKAIQ_INCLUDE_DIR}/algos
            ${RKAIQ_INCLUDE_DIR}/common
            ${RKAIQ_INCLUDE_DIR}/xcore
            ${ROCKIT_INCLUDE_DIR}
            ${ROCKIT_MPI_SDK_DIR}/lib/lib64
        )
        target_link_directories(CamVoViProbe PRIVATE ${ROCKIT_MPI_SDK_DIR}/lib/lib64)
        target_link_libraries(CamVoViProbe PRIVATE ${ROCKIT_LIBRARY} ${RKAIQ_LIBRARY} dl)
        set_target_properties(CamVoViProbe PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
    else ()
        message(STATUS "ROCKIT_MPI_SDK_DIR下未找到librockit.so或头文件,VI/VPSS/VO预览功能将以空实现编译")
    endif ()
else ()
    message(STATUS "未设置ROCKIT_MPI_SDK_DIR或rkaiq不可用,VI/VPSS/VO预览功能将以空实现编译(仅本机调试用)")
endif ()

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
