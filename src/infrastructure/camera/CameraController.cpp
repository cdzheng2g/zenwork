#include "infrastructure/camera/CameraController.h"
#include "contracts/FrameEnvelope.h"

#include <QDebug>

#include <opencv2/imgproc.hpp>

#include <chrono>
#include <cstring>

#ifdef HAVE_RKAIQ
// isp39.h内部会#define USE_NEWSTRUCT 1,决定rk_aiq_user_api2_ae.h等头文件
// 走新版ae_api_expSwAttr_t结构体分支(和参考实现cam_ctl_live.cpp的include顺序一致);
// 漏掉这个include会导致宏未定义,回退到旧结构体Uapi_ExpSwAttrV2_t,字段名完全不同
extern "C" {
#include <rk_aiq_user_api2_imgproc.h>
#include <rk_aiq_user_api2_sysctl.h>
#include <rk_aiq_user_api2_isp39.h>
}
#include <rk_aiq_user_api2_ae.h>
#include <rk_aiq_user_api2_sharp.h>
#endif

#ifdef HAVE_ROCKIT_MPI
extern "C" {
#include "rk_mpi_sys.h"
#include "rk_mpi_vi.h"
#include "rk_mpi_vpss.h"
#include "rk_mpi_vo.h"
#include "rk_mpi_mb.h"
#include "rk_comm_vo.h"
}

// Sensor原始采集分辨率,与CSI摄像头实际输出一致(参考cam_ctl_live.cpp的
// SENSOR_WIDTH/SENSOR_HEIGHT,该项目的sensor为1280x1024)。
static constexpr int kSensorWidth = 1280;
static constexpr int kSensorHeight = 1024;
// 触控屏物理分辨率(横屏1280x800),VO图层画布固定为整屏尺寸,局部叠加靠
// VO_CHN_ATTR_S.stRect裁剪到实际预览矩形(见voInit)。
static constexpr int kScreenWidth = 1280;
static constexpr int kScreenHeight = 800;
static constexpr int kViPipeId = 0;
static constexpr int kViChnId = 0;
static constexpr int kVpssGrpId = 0;
static constexpr int kVpssChnDisp = 0;
static constexpr int kVoDevId = 1;
static constexpr VO_LAYER kVoLayerId = VO_LAYER_CLUSTER0;
static constexpr int kVoChnId = 0;
#endif

CameraController& CameraController::instance()
{
    static CameraController inst;
    return inst;
}

// isp初始化,iqDir为rkaiq的iq文件目录(参考cam_ctl_live.cpp的rk_aiq_uapi2_sysctl_init)。
bool CameraController::initialize(int cameraId, const QString& iqDir)
{
    if (_initialized) {
        qInfo("CameraController: AIQ已经初始化，跳过重复初始化");
        return true;
    }

#ifdef HAVE_ROCKIT_MPI
    // 参考 cam_vo 的初始化顺序：MPI SYS -> AIQ -> VI/VPSS。
    if (!_mediaSystemInitialized) {
        if (RK_MPI_SYS_Init() != RK_SUCCESS) {
            qWarning("CameraController: SYS_Init失败，不能启动AIQ");
            return false;
        }
        _mediaSystemInitialized = true;
    }
#endif

#ifdef HAVE_RKAIQ
    rk_aiq_static_info_t info;
    memset(&info, 0, sizeof(info));

    if (rk_aiq_uapi2_sysctl_enumStaticMetasByPhyId(cameraId, &info) != XCAM_RETURN_NO_ERROR) {
        qWarning("CameraController: 获取sensor信息失败(cameraId=%d)", cameraId);
#ifdef HAVE_ROCKIT_MPI
        RK_MPI_SYS_Exit();
        _mediaSystemInitialized = false;
#endif
        return false;
    }

    qInfo(
        "CameraController: 初始化AIQ，sensor=%s，IQ目录=%s", info.sensor_info.sensor_name, qPrintable(iqDir));
    rk_aiq_uapi2_sysctl_preInit_scene(info.sensor_info.sensor_name, "normal", "day");

    rk_aiq_sys_ctx_t* ctx = rk_aiq_uapi2_sysctl_init(
        info.sensor_info.sensor_name, iqDir.toUtf8().constData(), nullptr, nullptr);
    if (!ctx) {
        qWarning("CameraController: AIQ初始化失败");
#ifdef HAVE_ROCKIT_MPI
        RK_MPI_SYS_Exit();
        _mediaSystemInitialized = false;
#endif
        return false;
    }

    if (rk_aiq_uapi2_sysctl_prepare(ctx, 0, 0, RK_AIQ_WORKING_MODE_NORMAL) != XCAM_RETURN_NO_ERROR
        || rk_aiq_uapi2_sysctl_start(ctx) != XCAM_RETURN_NO_ERROR) {
        qWarning("CameraController: AIQ prepare/start失败");
        rk_aiq_uapi2_sysctl_deinit(ctx);
#ifdef HAVE_ROCKIT_MPI
        RK_MPI_SYS_Exit();
        _mediaSystemInitialized = false;
#endif
        return false;
    }

    _aiqCtx = ctx;
    qInfo("CameraController: AIQ初始化并启动成功");
#else
    Q_UNUSED(cameraId)
    Q_UNUSED(iqDir)
#endif
    _initialized = true;
    return true;
}

void CameraController::shutdown()
{
    stopQtFrameStream();
    stopPreview();

#ifdef HAVE_RKAIQ
    if (_aiqCtx) {
        auto* ctx = static_cast<rk_aiq_sys_ctx_t*>(_aiqCtx);
        rk_aiq_uapi2_sysctl_stop(ctx, false);
        rk_aiq_uapi2_sysctl_deinit(ctx);
        _aiqCtx = nullptr;
    }
#endif
#ifdef HAVE_ROCKIT_MPI
    if (_mediaSystemInitialized) {
        RK_MPI_SYS_Exit();
        _mediaSystemInitialized = false;
    }
#endif
    _initialized = false;
}

#ifdef HAVE_RKAIQ
// 曝光/Sensor增益/ISP数字增益三者共用同一个ae_api_expSwAttr_t结构,任何一个
// 变化都要把另外两个的当前值一起写回去,否则会被重置为0(参考cam_ctl_live.cpp
// 的set_exposure/set_gain/set_isp_dgain三个函数的写法)。
static bool applyManualExposure(rk_aiq_sys_ctx_t* ctx, const CameraParams& params)
{
    ae_api_expSwAttr_t expAttr;
    memset(&expAttr, 0, sizeof(expAttr));

    if (rk_aiq_user_api2_ae_getExpSwAttr(ctx, &expAttr) != XCAM_RETURN_NO_ERROR)
        return false;

    expAttr.commCtrl.sw_aeT_opt_mode = RK_AIQ_OP_MODE_MANUAL;
    expAttr.commCtrl.meCtrl.linMe.sw_aeT_manTime_en = true;
    expAttr.commCtrl.meCtrl.linMe.sw_aeT_manTime_val = params.exposureMs / 1000.0f;
    expAttr.commCtrl.meCtrl.linMe.sw_aeT_manGain_en = true;
    expAttr.commCtrl.meCtrl.linMe.sw_aeT_manGain_val = params.gain;
    expAttr.commCtrl.meCtrl.linMe.sw_aeT_manIspDGain_en = true;
    expAttr.commCtrl.meCtrl.linMe.sw_aeT_manIspDGain_val = params.ispDGain;

    return rk_aiq_user_api2_ae_setExpSwAttr(ctx, expAttr) == XCAM_RETURN_NO_ERROR;
}
#endif

#ifdef HAVE_ROCKIT_MPI
// VI设备/管道/通道初始化,采集参数固定为传感器原始分辨率YUV420SP
// cam_ctl_live.cpp的vi_init,DevAttr全零表示sensor绑定由rkaiq的
// enumStaticMetasByPhyId间接完成,VI层不需要额外配置)。
static bool viInit()
{
    VI_DEV_ATTR_S devAttr;
    VI_DEV_BIND_PIPE_S bindPipe;
    VI_CHN_ATTR_S chnAttr;
    memset(&devAttr, 0, sizeof(devAttr));
    memset(&bindPipe, 0, sizeof(bindPipe));
    memset(&chnAttr, 0, sizeof(chnAttr));
    // 初始化 VI设备属性,如果没有配置过则设置默认值
    const int devAttrResult = RK_MPI_VI_GetDevAttr(kViPipeId, &devAttr);
    if (devAttrResult == RK_ERR_VI_NOT_CONFIG) {
        const int result = RK_MPI_VI_SetDevAttr(kViPipeId, &devAttr);
        if (result != RK_SUCCESS) {
            qWarning("CameraController: VI_SetDevAttr失败, ret=%#x", result);
            return false;
        }
    }
    // 启用 VI设备,如果没有启用则启用
    const int deviceEnabledResult = RK_MPI_VI_GetDevIsEnable(kViPipeId);
    if (deviceEnabledResult != RK_SUCCESS) {
        const int enableDeviceResult = RK_MPI_VI_EnableDev(kViPipeId);
        if (enableDeviceResult != RK_SUCCESS) {
            qWarning("CameraController: VI_EnableDev失败, ret=%#x", enableDeviceResult);
            return false;
        }
        // 绑定 VI设备和管道,如果没有绑定则绑定
        //  One bound pipe; u32Num is a count, not the pipe ID.
        bindPipe.u32Num = 1;
        bindPipe.PipeId[0] = kViPipeId;
        const int bindPipeResult = RK_MPI_VI_SetDevBindPipe(kViPipeId, &bindPipe);
        if (bindPipeResult != RK_SUCCESS) {
            qWarning("CameraController: VI_SetDevBindPipe失败, ret=%#x", bindPipeResult);
            return false;
        }
    }

    chnAttr.stIspOpt.u32BufCount = 10;
    // Use native multi-planar DMABUF through Rockit VI.
    chnAttr.stIspOpt.enMemoryType = VI_V4L2_MEMORY_TYPE_DMABUF;
    // rkisp_mainpath is multi-planar. The libv4l2 compatibility plugin
    // translates capture buffers but not EXPBUF, causing EINVAL in Rockit.
    chnAttr.stIspOpt.bNoUseLibV4L2 = RK_TRUE;
    chnAttr.stIspOpt.enCaptureType = VI_V4L2_CAPTURE_TYPE_VIDEO_CAPTURE_MPLANE;
    chnAttr.enAllocBufType = VI_ALLOC_BUF_TYPE_INTERNAL;
    chnAttr.stSize.u32Width = kSensorWidth;
    chnAttr.stSize.u32Height = kSensorHeight;
    chnAttr.enPixelFormat = RK_FMT_YUV420SP;
    // Rockit的VI通道不能用零帧率；-1表示不限制、跟随sensor实际输出帧率。
    // 未显式设置时memset留下0，EnableChn会以illegal param失败。
    chnAttr.stFrameRate.s32SrcFrameRate = -1;
    chnAttr.stFrameRate.s32DstFrameRate = -1;
    chnAttr.u32Depth = 2;

    const int setChannelResult = RK_MPI_VI_SetChnAttr(kViPipeId, kViChnId, &chnAttr);
    if (setChannelResult != RK_SUCCESS) {
        qWarning("CameraController: VI_SetChnAttr失败, ret=%#x", setChannelResult);
        return false;
    }

    const int enableChannelResult = RK_MPI_VI_EnableChn(kViPipeId, kViChnId);
    if (enableChannelResult != RK_SUCCESS) {
        qWarning("CameraController: VI_EnableChn失败, ret=%#x, devAttr=%#x, devEnabled=%#x, size=%dx%d, "
                 "format=NV12",
            enableChannelResult, devAttrResult, deviceEnabledResult, kSensorWidth, kSensorHeight);
        return false;
    }

    return true;
}

// VPSS分组/通道初始化并与VI硬件绑定,通道输出尺寸缩放到预览矩形大小
// (参考cam_ctl_live.cpp的vpss_init,VO_WIDTH/VO_HEIGHT换成rect.width/height)。
static bool vpssInit(const PreviewRect& rect)
{
    VPSS_GRP_ATTR_S grpAttr;
    VPSS_CHN_ATTR_S chnAttr;
    memset(&grpAttr, 0, sizeof(grpAttr));
    memset(&chnAttr, 0, sizeof(chnAttr));

    grpAttr.u32MaxW = kSensorWidth;
    grpAttr.u32MaxH = kSensorHeight;
    grpAttr.enPixelFormat = RK_FMT_YUV420SP;
    grpAttr.stFrameRate.s32SrcFrameRate = -1;
    grpAttr.stFrameRate.s32DstFrameRate = -1;
    grpAttr.enCompressMode = COMPRESS_MODE_NONE;

    if (RK_MPI_VPSS_CreateGrp(kVpssGrpId, &grpAttr) != RK_SUCCESS) {
        qWarning("CameraController: VPSS_CreateGrp失败");
        return false;
    }

    chnAttr.enChnMode = VPSS_CHN_MODE_AUTO;
    chnAttr.enDynamicRange = DYNAMIC_RANGE_SDR8;
    chnAttr.enPixelFormat = RK_FMT_YUV420SP;
    chnAttr.stFrameRate.s32SrcFrameRate = -1;
    chnAttr.stFrameRate.s32DstFrameRate = -1;
    chnAttr.u32Width = rect.width;
    chnAttr.u32Height = rect.height;
    chnAttr.enCompressMode = COMPRESS_MODE_NONE;
    chnAttr.u32Depth = 1;
    chnAttr.u32FrameBufCnt = 10;

    if (RK_MPI_VPSS_SetChnAttr(kVpssGrpId, kVpssChnDisp, &chnAttr) != RK_SUCCESS) {
        qWarning("CameraController: VPSS_SetChnAttr失败");
        return false;
    }

    if (RK_MPI_VPSS_EnableChn(kVpssGrpId, kVpssChnDisp) != RK_SUCCESS) {
        qWarning("CameraController: VPSS_EnableChn失败");
        return false;
    }

    RK_MPI_VPSS_SetVProcDev(kVpssGrpId, VIDEO_PROC_DEV_GPU);

    if (RK_MPI_VPSS_StartGrp(kVpssGrpId) != RK_SUCCESS) {
        qWarning("CameraController: VPSS_StartGrp失败");
        return false;
    }

    MPP_CHN_S viChn;
    MPP_CHN_S vpssChn;
    memset(&viChn, 0, sizeof(viChn));
    memset(&vpssChn, 0, sizeof(vpssChn));
    viChn.enModId = RK_ID_VI;
    viChn.s32DevId = 0;
    viChn.s32ChnId = kViChnId;
    vpssChn.enModId = RK_ID_VPSS;
    vpssChn.s32DevId = 0;
    vpssChn.s32ChnId = kVpssGrpId;

    if (RK_MPI_SYS_Bind(&viChn, &vpssChn) != RK_SUCCESS) {
        qWarning("CameraController: SYS_Bind VI->VPSS失败");
        return false;
    }

    return true;
}

// VO设备/图层/通道初始化,图层画布保持全屏(屏幕物理分辨率),通道矩形
// (stRect)设为预览目标矩形,实现局部叠加而不是全屏显示(参考cam_ctl_live.cpp
// 的vo_init,layer.stDispRect/stImageSize换成屏幕整屏尺寸,chn.stRect换成rect)。
static bool voInit(const PreviewRect& rect)
{
    VO_PUB_ATTR_S pub;
    VO_VIDEO_LAYER_ATTR_S layer;
    VO_CHN_ATTR_S chn;
    memset(&pub, 0, sizeof(pub));
    memset(&layer, 0, sizeof(layer));
    memset(&chn, 0, sizeof(chn));

    pub.enIntfType = VO_INTF_MIPI;
    pub.enIntfSync = VO_OUTPUT_DEFAULT;

    if (RK_MPI_VO_SetPubAttr(kVoDevId, &pub) != RK_SUCCESS) {
        qWarning("CameraController: VO_SetPubAttr失败");
        return false;
    }

    if (RK_MPI_VO_Enable(kVoDevId) != RK_SUCCESS) {
        qWarning("CameraController: VO_Enable失败");
        return false;
    }

    layer.stDispRect.s32X = 0;
    layer.stDispRect.s32Y = 0;
    layer.stDispRect.u32Width = kScreenWidth;
    layer.stDispRect.u32Height = kScreenHeight;
    layer.stImageSize.u32Width = kScreenWidth;
    layer.stImageSize.u32Height = kScreenHeight;
    layer.u32DispFrmRt = 60;
    layer.enPixFormat = RK_FMT_YUV420SP;

    if (RK_MPI_VO_BindLayer(kVoLayerId, kVoDevId, VO_LAYER_MODE_VIDEO) != RK_SUCCESS) {
        qWarning("CameraController: VO_BindLayer失败");
        return false;
    }

    if (RK_MPI_VO_SetLayerAttr(kVoLayerId, &layer) != RK_SUCCESS) {
        qWarning("CameraController: VO_SetLayerAttr失败");
        return false;
    }

    if (RK_MPI_VO_SetLayerSpliceMode(kVoLayerId, VO_SPLICE_MODE_RGA) != RK_SUCCESS) {
        qWarning("CameraController: VO_SetLayerSpliceMode失败");
        return false;
    }

    if (RK_MPI_VO_EnableLayer(kVoLayerId) != RK_SUCCESS) {
        qWarning("CameraController: VO_EnableLayer失败");
        return false;
    }

    chn.bDeflicker = RK_FALSE;
    chn.u32Priority = 0;
    chn.stRect.s32X = rect.x;
    chn.stRect.s32Y = rect.y;
    chn.stRect.u32Width = rect.width;
    chn.stRect.u32Height = rect.height;

    if (RK_MPI_VO_SetChnAttr(kVoLayerId, kVoChnId, &chn) != RK_SUCCESS) {
        qWarning("CameraController: VO_SetChnAttr失败");
        return false;
    }

    if (RK_MPI_VO_EnableChn(kVoLayerId, kVoChnId) != RK_SUCCESS) {
        qWarning("CameraController: VO_EnableChn失败");
        return false;
    }

    return true;
}

// 按VI->VPSS->VO的逆序释放资源,running标记须由调用方保证此时轮询线程已退出
// (参考cam_ctl_live.cpp的cleanup,pthread_join之后才做MPI资源释放)。
static void teardownPipeline()
{
    RK_MPI_VO_DisableChn(kVoLayerId, kVoChnId);
    RK_MPI_VO_DisableLayer(kVoLayerId);
    RK_MPI_VO_Disable(kVoDevId);

    MPP_CHN_S viChn;
    MPP_CHN_S vpssChn;
    memset(&viChn, 0, sizeof(viChn));
    memset(&vpssChn, 0, sizeof(vpssChn));
    viChn.enModId = RK_ID_VI;
    viChn.s32DevId = 0;
    viChn.s32ChnId = kViChnId;
    vpssChn.enModId = RK_ID_VPSS;
    vpssChn.s32DevId = 0;
    vpssChn.s32ChnId = kVpssGrpId;
    RK_MPI_SYS_UnBind(&viChn, &vpssChn);

    RK_MPI_VPSS_StopGrp(kVpssGrpId);
    RK_MPI_VPSS_DisableChn(kVpssGrpId, kVpssChnDisp);
    RK_MPI_VPSS_DestroyGrp(kVpssGrpId);

    RK_MPI_VI_DisableChn(kViPipeId, kViChnId);
    RK_MPI_VI_DisableDev(kViPipeId);
}

// Qt预览不使用VO硬件叠加，只释放采集的VI->VPSS链路。
static void teardownFramePipeline()
{
    MPP_CHN_S viChn;
    MPP_CHN_S vpssChn;
    memset(&viChn, 0, sizeof(viChn));
    memset(&vpssChn, 0, sizeof(vpssChn));
    viChn.enModId = RK_ID_VI;
    viChn.s32DevId = 0;
    viChn.s32ChnId = kViChnId;
    vpssChn.enModId = RK_ID_VPSS;
    vpssChn.s32DevId = 0;
    vpssChn.s32ChnId = kVpssGrpId;

    RK_MPI_SYS_UnBind(&viChn, &vpssChn);
    RK_MPI_VPSS_StopGrp(kVpssGrpId);
    RK_MPI_VPSS_DisableChn(kVpssGrpId, kVpssChnDisp);
    RK_MPI_VPSS_DestroyGrp(kVpssGrpId);
    RK_MPI_VI_DisableChn(kViPipeId, kViChnId);
    RK_MPI_VI_DisableDev(kViPipeId);
}

// VPSS->VO软件轮询转发线程:GetChnFrame阻塞最多500ms取一帧,取到就直接
// SendFrame给VO再ReleaseChnFrame,取不到就短暂休眠重试(参考cam_ctl_live.cpp
// 的vo_run_thread)。running为false时退出循环。
static void voPollThread(std::atomic<bool>* running)
{
    VIDEO_FRAME_INFO_S frame;

    while (running->load()) {
        int ret = RK_MPI_VPSS_GetChnFrame(kVpssGrpId, kVpssChnDisp, &frame, 500);
        if (ret == RK_SUCCESS) {
            RK_MPI_VO_SendFrame(kVoLayerId, kVoChnId, &frame, -1);
            RK_MPI_VPSS_ReleaseChnFrame(kVpssGrpId, kVpssChnDisp, &frame);
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(2));
        }
    }
}
#endif // HAVE_ROCKIT_MPI

bool CameraController::startQtFrameStream(int width, int height)
{
    if (width <= 0 || height <= 0 || !_initialized)
        return false;

#ifdef HAVE_ROCKIT_MPI
    if (_qtFrameStreamRunning)
        return _qtFrameRect.width == width && _qtFrameRect.height == height;

    const PreviewRect frameRect { 0, 0, width, height };
    if (!viInit() || !vpssInit(frameRect)) {
        teardownFramePipeline();
        return false;
    }

    _qtFrameRect = frameRect;
    _qtFrameStreamRunning = true;
    qInfo("CameraController: Qt预览VI->VPSS取帧已启动(%dx%d)", width, height);
    return true;
#else
    Q_UNUSED(width)
    Q_UNUSED(height)
    return false;
#endif
}

bool CameraController::readQtPreviewFrame(cv::Mat& frame, int timeoutMs, qint64* acquiredAtNs)
{
    frame.release();

#ifdef HAVE_ROCKIT_MPI
    if (!_qtFrameStreamRunning)
        return false;

    VIDEO_FRAME_INFO_S nativeFrame;
    memset(&nativeFrame, 0, sizeof(nativeFrame));
    const int result = RK_MPI_VPSS_GetChnFrame(kVpssGrpId, kVpssChnDisp, &nativeFrame, timeoutMs);
    if (result != RK_SUCCESS)
        return false;
    if (acquiredAtNs) *acquiredAtNs = monotonicNowNs();

    struct FrameRelease {
        VIDEO_FRAME_INFO_S* frame;
        ~FrameRelease()
        {
            RK_MPI_VPSS_ReleaseChnFrame(kVpssGrpId, kVpssChnDisp, frame);
        }
    } release { &nativeFrame };

    const auto& sourceFrame = nativeFrame.stVFrame;
    // Match the SDK VPSS example: invalidate CPU cache before reading DMA output.
    if (RK_MPI_SYS_MmzFlushCache(sourceFrame.pMbBlk, RK_TRUE) != RK_SUCCESS)
        return false;
    const int width = static_cast<int>(sourceFrame.u32Width);
    const int height = static_cast<int>(sourceFrame.u32Height);
    const int stride = static_cast<int>(sourceFrame.u32VirWidth);
    const auto* source = static_cast<const unsigned char*>(RK_MPI_MB_Handle2VirAddr(sourceFrame.pMbBlk));
    if (!source || width <= 0 || height <= 0 || stride < width)
        return false;

    // NV12在Rockit缓冲中的UV平面从对齐后的Y平面末尾开始，不能把stride
    // 当作可见宽度直接交给OpenCV，否则会出现绿色、发黑或图像错行。
    cv::Mat nv12(height + height / 2, width, CV_8UC1);
    const size_t yPlaneBytes = static_cast<size_t>(stride) * sourceFrame.u32VirHeight;
    for (int row = 0; row < height; ++row) {
        memcpy(nv12.ptr(row), source + static_cast<size_t>(row) * stride, width);
    }
    const auto* uv = source + yPlaneBytes;
    for (int row = 0; row < height / 2; ++row) {
        memcpy(nv12.ptr(height + row), uv + static_cast<size_t>(row) * stride, width);
    }

    cv::cvtColor(nv12, frame, cv::COLOR_YUV2BGR_NV12);
    return !frame.empty();
#else
    Q_UNUSED(timeoutMs)
    Q_UNUSED(acquiredAtNs)
    return false;
#endif
}

void CameraController::stopQtFrameStream()
{
#ifdef HAVE_ROCKIT_MPI
    if (!_qtFrameStreamRunning)
        return;

    _qtFrameStreamRunning = false;
    teardownFramePipeline();
    qInfo("CameraController: Qt预览VI->VPSS取帧已停止");
#endif
}

bool CameraController::startPreview(const PreviewRect& rect)
{
    if (rect.width <= 0 || rect.height <= 0)
        return false;

#ifdef HAVE_ROCKIT_MPI
    if (!_initialized)
        return false;

    if (_previewRunning)
        stopPreview();

    if (!viInit() || !vpssInit(rect) || !voInit(rect)) {
        teardownPipeline();
        return false;
    }

    _previewRect = rect;
    _previewRunning = true;
    _voThread = std::thread(voPollThread, &_previewRunning);
    return true;
#else
    Q_UNUSED(rect)
    return false;
#endif
}

void CameraController::stopPreview()
{
#ifdef HAVE_ROCKIT_MPI
    if (!_previewRunning)
        return;

    _previewRunning = false;
    if (_voThread.joinable())
        _voThread.join();

    teardownPipeline();
#endif
}

bool CameraController::setExposure(double exposureMs)
{
    if (exposureMs <= 0.0)
        return false;

    _params.exposureMs = exposureMs;
    _params.autoExposure = false;

#ifdef HAVE_RKAIQ
    if (!_aiqCtx)
        return false;
    return applyManualExposure(static_cast<rk_aiq_sys_ctx_t*>(_aiqCtx), _params);
#else
    return true;
#endif
}

bool CameraController::setGain(double gain)
{
    if (gain <= 0.0)
        return false;

    _params.gain = gain;
    _params.autoExposure = false;

#ifdef HAVE_RKAIQ
    if (!_aiqCtx)
        return false;
    return applyManualExposure(static_cast<rk_aiq_sys_ctx_t*>(_aiqCtx), _params);
#else
    return true;
#endif
}

bool CameraController::setIspDGain(double ispDGain)
{
    if (ispDGain <= 0.0)
        return false;

    _params.ispDGain = ispDGain;
    _params.autoExposure = false;

#ifdef HAVE_RKAIQ
    if (!_aiqCtx)
        return false;
    return applyManualExposure(static_cast<rk_aiq_sys_ctx_t*>(_aiqCtx), _params);
#else
    return true;
#endif
}

bool CameraController::setAutoExposure()
{
    _params.autoExposure = true;

#ifdef HAVE_RKAIQ
    if (!_aiqCtx)
        return false;

    ae_api_expSwAttr_t expAttr;
    memset(&expAttr, 0, sizeof(expAttr));
    auto* ctx = static_cast<rk_aiq_sys_ctx_t*>(_aiqCtx);

    if (rk_aiq_user_api2_ae_getExpSwAttr(ctx, &expAttr) != XCAM_RETURN_NO_ERROR)
        return false;

    expAttr.commCtrl.sw_aeT_opt_mode = RK_AIQ_OP_MODE_AUTO;
    return rk_aiq_user_api2_ae_setExpSwAttr(ctx, expAttr) == XCAM_RETURN_NO_ERROR;
#else
    return true;
#endif
}

bool CameraController::setContrast(int contrast)
{
    if (contrast < 0 || contrast > 255)
        return false;

    _params.contrast = contrast;

#ifdef HAVE_RKAIQ
    if (!_aiqCtx)
        return false;
    return rk_aiq_uapi2_setContrast(static_cast<rk_aiq_sys_ctx_t*>(_aiqCtx), contrast)
        == XCAM_RETURN_NO_ERROR;
#else
    return true;
#endif
}

bool CameraController::setSaturation(int saturation)
{
    if (saturation < 0 || saturation > 255)
        return false;

    _params.saturation = saturation;

#ifdef HAVE_RKAIQ
    if (!_aiqCtx)
        return false;
    return rk_aiq_uapi2_setSaturation(static_cast<rk_aiq_sys_ctx_t*>(_aiqCtx), saturation)
        == XCAM_RETURN_NO_ERROR;
#else
    return true;
#endif
}

bool CameraController::setSharpness(double sharpness)
{
    if (sharpness < 0.0 || sharpness > 100.0)
        return false;

    _params.sharpness = sharpness;

#ifdef HAVE_RKAIQ
    if (!_aiqCtx)
        return false;

    asharp_strength_t strength;
    memset(&strength, 0, sizeof(strength));
    strength.en = true;
    strength.percent = sharpness;

    return rk_aiq_user_api2_sharp_SetStrength(static_cast<rk_aiq_sys_ctx_t*>(_aiqCtx), &strength)
        == XCAM_RETURN_NO_ERROR;
#else
    return true;
#endif
}

bool CameraController::applyParams(const CameraParams& params)
{
    bool ok = true;

    ok = setContrast(params.contrast) && ok;
    ok = setSaturation(params.saturation) && ok;
    ok = setSharpness(params.sharpness) && ok;

    if (params.autoExposure) {
        ok = setAutoExposure() && ok;
    } else {
        // 曝光/增益/ISP增益共用同一结构,应用完整组合时先落好数值再统一写入一次,
        // 避免逐个setter各自触发一次AE属性读写(参考applyManualExposure的说明)。
        _params.exposureMs = params.exposureMs;
        _params.gain = params.gain;
        _params.ispDGain = params.ispDGain;
        _params.autoExposure = false;

#ifdef HAVE_RKAIQ
        if (!_aiqCtx) {
            ok = false;
        } else {
            ok = applyManualExposure(static_cast<rk_aiq_sys_ctx_t*>(_aiqCtx), _params) && ok;
        }
#endif
    }

    return ok;
}
