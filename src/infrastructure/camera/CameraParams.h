#ifndef CAMERAPARAMS_H
#define CAMERAPARAMS_H

// 相机成像参数:对应RK3576 ISP(rkaiq)的曝光/增益/画质调节能力。
// 曝光相关字段在autoExposure为true时仅作显示用,实际由ISP自动算法决定;
// 为false时exposureMs/gain/ispDGain三者联动生效(参考cam_ctl_live.cpp的set_exposure)。
struct CameraParams {
    bool autoExposure { true };
    double exposureMs { 10.0 }; // 曝光时间,单位ms
    double gain { 1.0 }; // Sensor模拟增益,倍数
    double ispDGain { 1.0 }; // ISP数字增益,倍数

    int contrast { 128 }; // 对比度,0~255
    int saturation { 128 }; // 饱和度,0~255
    double sharpness { 100.0 }; // 锐度,0~100(百分比)
};

#endif // CAMERAPARAMS_H
