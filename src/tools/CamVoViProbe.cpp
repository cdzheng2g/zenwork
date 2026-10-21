// 独立诊断程序：按当前应用的 AIQ -> VI -> VPSS 采集配置运行。
// 不创建Qt窗口、不占用VO，成功标准是在VPSS通道0取到一帧。

#include <cstdio>
#include <cstring>

extern "C" {
#include <rk_aiq_user_api2_sysctl.h>
#include "rk_mpi_sys.h"
#include "rk_mpi_vi.h"
#include "rk_mpi_vpss.h"
}

namespace {
constexpr int kWidth = 1280;
constexpr int kHeight = 1024;
constexpr int kDisplayWidth = 1024;
constexpr int kDisplayHeight = 768;
constexpr int kPipe = 0;
constexpr int kChannel = 0;
constexpr int kVpssGroup = 0;
constexpr int kVpssChannel = 0;

rk_aiq_sys_ctx_t* gAiq = nullptr;
bool gViEnabled = false;
bool gVpssCreated = false;
bool gVpssStarted = false;
bool gBound = false;

void cleanup()
{
    MPP_CHN_S vi { };
    MPP_CHN_S vpss { };
    vi.enModId = RK_ID_VI;
    vi.s32DevId = 0;
    vi.s32ChnId = kChannel;
    vpss.enModId = RK_ID_VPSS;
    vpss.s32DevId = 0;
    vpss.s32ChnId = kVpssGroup;
    if (gBound)
        RK_MPI_SYS_UnBind(&vi, &vpss);
    if (gVpssStarted)
        RK_MPI_VPSS_StopGrp(kVpssGroup);
    if (gVpssCreated) {
        RK_MPI_VPSS_DisableChn(kVpssGroup, kVpssChannel);
        RK_MPI_VPSS_DestroyGrp(kVpssGroup);
    }
    if (gViEnabled) {
        RK_MPI_VI_DisableChn(kPipe, kChannel);
        RK_MPI_VI_DisableDev(kPipe);
    }
    if (gAiq) {
        rk_aiq_uapi2_sysctl_stop(gAiq, false);
        rk_aiq_uapi2_sysctl_deinit(gAiq);
    }
    RK_MPI_SYS_Exit();
}

int run()
{
    if (RK_MPI_SYS_Init() != RK_SUCCESS) {
        std::printf("PROBE: SYS_Init failed\n");
        return 1;
    }

    rk_aiq_static_info_t info { };
    const XCamReturn metaResult = rk_aiq_uapi2_sysctl_enumStaticMetasByPhyId(0, &info);
    if (metaResult != XCAM_RETURN_NO_ERROR) {
        std::printf("PROBE: enum sensor failed: %d\n", metaResult);
        return 2;
    }
    rk_aiq_uapi2_sysctl_preInit_scene(info.sensor_info.sensor_name, "normal", "day");
    gAiq = rk_aiq_uapi2_sysctl_init(info.sensor_info.sensor_name, "/etc/iqfiles", nullptr, nullptr);
    if (!gAiq || rk_aiq_uapi2_sysctl_prepare(gAiq, 0, 0, RK_AIQ_WORKING_MODE_NORMAL) != XCAM_RETURN_NO_ERROR
        || rk_aiq_uapi2_sysctl_start(gAiq) != XCAM_RETURN_NO_ERROR) {
        std::printf("PROBE: AIQ start failed\n");
        return 3;
    }
    std::printf("PROBE: AIQ ok, sensor=%s\n", info.sensor_info.sensor_name);

    // Use the same native multi-planar DMABUF VI configuration as the app.
    VI_DEV_ATTR_S deviceAttr { };
    VI_DEV_BIND_PIPE_S bindPipe { };
    VI_CHN_ATTR_S channelAttr { };
    int ret = RK_MPI_VI_GetDevAttr(kPipe, &deviceAttr);
    if (ret == RK_ERR_VI_NOT_CONFIG) {
        ret = RK_MPI_VI_SetDevAttr(kPipe, &deviceAttr);
        if (ret) {
            std::printf("PROBE: VI_SetDevAttr ret=%#x\n", ret);
            return 4;
        }
    }
    if (RK_MPI_VI_GetDevIsEnable(kPipe) != RK_SUCCESS) {
        ret = RK_MPI_VI_EnableDev(kPipe);
        if (ret) {
            std::printf("PROBE: VI_EnableDev ret=%#x\n", ret);
            return 5;
        }
        // Bind one VI pipe.
        bindPipe.u32Num = 1;
        bindPipe.PipeId[0] = kPipe;
        ret = RK_MPI_VI_SetDevBindPipe(kPipe, &bindPipe);
        if (ret) {
            std::printf("PROBE: VI_SetDevBindPipe ret=%#x\n", ret);
            return 6;
        }
    }
    channelAttr.stIspOpt.u32BufCount = 10;
    channelAttr.stIspOpt.enMemoryType = VI_V4L2_MEMORY_TYPE_DMABUF;
    // rkisp_mainpath is multi-planar. The libv4l2 compatibility plugin
    // translates capture buffers but not EXPBUF, causing EINVAL in Rockit.
    channelAttr.stIspOpt.bNoUseLibV4L2 = RK_TRUE;
    channelAttr.stIspOpt.enCaptureType = VI_V4L2_CAPTURE_TYPE_VIDEO_CAPTURE_MPLANE;
    channelAttr.enAllocBufType = VI_ALLOC_BUF_TYPE_INTERNAL;
    channelAttr.stSize.u32Width = kWidth;
    channelAttr.stSize.u32Height = kHeight;
    channelAttr.enPixelFormat = RK_FMT_YUV420SP;
    channelAttr.stFrameRate.s32SrcFrameRate = -1;
    channelAttr.stFrameRate.s32DstFrameRate = -1;
    channelAttr.u32Depth = 2;
    ret = RK_MPI_VI_SetChnAttr(kPipe, kChannel, &channelAttr);
    if (ret) {
        std::printf("PROBE: VI_SetChnAttr ret=%#x\n", ret);
        return 7;
    }
    ret = RK_MPI_VI_EnableChn(kPipe, kChannel);
    if (ret) {
        std::printf("PROBE: VI_EnableChn ret=%#x\n", ret);
        return 8;
    }
    gViEnabled = true;
    std::printf("PROBE: VI ok\n");

    VPSS_GRP_ATTR_S groupAttr { };
    VPSS_CHN_ATTR_S outputAttr { };
    groupAttr.u32MaxW = kWidth;
    groupAttr.u32MaxH = kHeight;
    groupAttr.enPixelFormat = RK_FMT_YUV420SP;
    groupAttr.stFrameRate.s32SrcFrameRate = -1;
    groupAttr.stFrameRate.s32DstFrameRate = -1;
    groupAttr.enCompressMode = COMPRESS_MODE_NONE;
    ret = RK_MPI_VPSS_CreateGrp(kVpssGroup, &groupAttr);
    if (ret) {
        std::printf("PROBE: VPSS_CreateGrp ret=%#x\n", ret);
        return 9;
    }
    gVpssCreated = true;

    outputAttr.enChnMode = VPSS_CHN_MODE_AUTO;
    outputAttr.enDynamicRange = DYNAMIC_RANGE_SDR8;
    outputAttr.enPixelFormat = RK_FMT_YUV420SP;
    outputAttr.stFrameRate.s32SrcFrameRate = -1;
    outputAttr.stFrameRate.s32DstFrameRate = -1;
    outputAttr.u32Width = kDisplayWidth;
    outputAttr.u32Height = kDisplayHeight;
    outputAttr.enCompressMode = COMPRESS_MODE_NONE;
    outputAttr.u32Depth = 1;
    outputAttr.u32FrameBufCnt = 10;
    ret = RK_MPI_VPSS_SetChnAttr(kVpssGroup, kVpssChannel, &outputAttr);
    if (ret) {
        std::printf("PROBE: VPSS_SetChnAttr ret=%#x\n", ret);
        return 10;
    }
    ret = RK_MPI_VPSS_EnableChn(kVpssGroup, kVpssChannel);
    if (ret) {
        std::printf("PROBE: VPSS_EnableChn ret=%#x\n", ret);
        return 11;
    }
    RK_MPI_VPSS_SetVProcDev(kVpssGroup, VIDEO_PROC_DEV_GPU);
    ret = RK_MPI_VPSS_StartGrp(kVpssGroup);
    if (ret) {
        std::printf("PROBE: VPSS_StartGrp ret=%#x\n", ret);
        return 12;
    }
    gVpssStarted = true;

    MPP_CHN_S vi { RK_ID_VI, 0, kChannel };
    MPP_CHN_S vpss { RK_ID_VPSS, 0, kVpssGroup };
    ret = RK_MPI_SYS_Bind(&vi, &vpss);
    if (ret) {
        std::printf("PROBE: SYS_Bind ret=%#x\n", ret);
        return 13;
    }
    gBound = true;

    VIDEO_FRAME_INFO_S frame { };
    ret = RK_MPI_VPSS_GetChnFrame(kVpssGroup, kVpssChannel, &frame, 3000);
    if (ret) {
        std::printf("PROBE: VPSS_GetChnFrame ret=%#x\n", ret);
        return 14;
    }
    std::printf("PROBE: SUCCESS frame=%ux%u stride=%u\n", frame.stVFrame.u32Width, frame.stVFrame.u32Height,
        frame.stVFrame.u32VirWidth);
    RK_MPI_VPSS_ReleaseChnFrame(kVpssGroup, kVpssChannel, &frame);
    return 0;
}
} // namespace

int main()
{
    const int result = run();
    cleanup();
    return result;
}
