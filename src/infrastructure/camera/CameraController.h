#ifndef CAMERACONTROLLER_H
#define CAMERACONTROLLER_H

#include "infrastructure/camera/CameraParams.h"

#include <opencv2/core.hpp>

#include <QString>

#include <atomic>
#include <thread>

// 预览画面在屏幕上的目标矩形(设备无关像素,来自ImageCanvasWidget::mapToGlobal
// 换算后的屏幕物理坐标),用于VO层局部矩形叠加。
struct PreviewRect {
    int x { 0 };
    int y { 0 };
    int width { 0 };
    int height { 0 };
};

// 相机ISP参数控制 + VI/VPSS/VO硬件叠加预览的硬件接口层,封装对RK3576
// rkaiq/rockit MPI API的调用(参考cam_ctl_live.cpp的set_exposure/set_gain/
// set_isp_dgain/set_contrast/set_saturation/set_sharpness/set_auto_exposure以及
// vi_init/vpss_init/vo_init/vo_run_thread/cleanup)。
//
// 本机x86开发环境没有rkaiq/rockit SDK与ISP硬件,交叉编译到RK3576板端时CMake会
// 定义HAVE_RKAIQ/HAVE_ROCKIT_MPI宏并链接对应库,此时.cpp中的实现分支才会真正
// 调用ISP/MPI API;未定义这些宏时所有方法只更新内存态并返回true(ISP参数)或
// false(预览,因为没有真实硬件可显示),方便本机开发/调试UI而不依赖真实硬件。
class CameraController {
public:
    static CameraController& instance();

    // 用指定摄像头初始化ISP上下文,cameraId对应rk_aiq_uapi2_sysctl_init的sensor
    // 物理编号,iqDir为IQ文件目录(默认/etc/iqfiles)。未接入硬件(无HAVE_RKAIQ)
    // 时直接返回true,不做任何实际操作。
    bool initialize(int cameraId = 0, const QString& iqDir = QStringLiteral("/etc/iqfiles"));
    void shutdown();
    bool isInitialized() const
    {
        return _initialized;
    }

    // 启动VI->VPSS->VO硬件叠加预览,rect为预览画面在屏幕上的目标矩形(局部叠加,
    // 不是全屏)。需要先initialize()成功。重复调用会先停止已有预览再按新矩形
    // 重新启动。未链接rockit MPI SDK(无HAVE_ROCKIT_MPI)时直接返回false。
    bool startPreview(const PreviewRect& rect);
    // 停止预览并释放VI/VPSS/VO资源,重复调用/未启动时安全地什么都不做。
    void stopPreview();
    bool isPreviewRunning() const
    {
        return _previewRunning;
    }

    // Qt预览专用的取帧通道。它和AIQ共用同一个VI->VPSS采集链路，但不会创建
    // VO硬件图层，以免覆盖正在运行的Qt桌面。readPreviewFrame()返回独立持有的
    // BGR图像，可安全地经Qt排队信号交给界面线程。
    bool startQtFrameStream(int width, int height);
    bool readQtPreviewFrame(cv::Mat& frame, int timeoutMs, qint64* acquiredAtNs = nullptr);
    void stopQtFrameStream();
    bool isQtFrameStreamRunning() const
    {
        return _qtFrameStreamRunning;
    }

    bool setExposure(double exposureMs);
    bool setGain(double gain);
    bool setIspDGain(double ispDGain);
    bool setAutoExposure();
    bool setContrast(int contrast);
    bool setSaturation(int saturation);
    bool setSharpness(double sharpness);

    // 一次性应用一整套参数(用于加载配置/切换型号后恢复)
    bool applyParams(const CameraParams& params);

    const CameraParams& currentParams() const
    {
        return _params;
    }

private:
    CameraController() = default;

    void* _aiqCtx { nullptr };
    bool _initialized { false };
    bool _mediaSystemInitialized { false };
    CameraParams _params;

    std::thread _voThread;
    std::atomic<bool> _previewRunning { false };
    PreviewRect _previewRect;
    std::atomic<bool> _qtFrameStreamRunning { false };
    PreviewRect _qtFrameRect;
};

#endif // CAMERACONTROLLER_H
