#ifndef CAMERAPARAMSSTORE_H
#define CAMERAPARAMSSTORE_H

#include "infrastructure/camera/CameraParams.h"

#include <QString>

// 相机成像参数的持久化:按产品型号名分别保存一份CameraParams到磁盘,
// 落地在 AppDataLocation/camera_params/<modelName>.json 下,和
// ReferenceImageManager按型号目录管理基准图是同一套思路,方便切换型号时
// 自动套用对应参数组合。型号名为空时使用"default"作为通用配置。
class CameraParamsStore {
public:
    static CameraParamsStore& instance();

    bool save(const QString& modelName, const CameraParams& params) const;
    // 若modelName没有保存过配置,返回false且params保持不变(调用方应保留当前值)
    bool load(const QString& modelName, CameraParams& params) const;

private:
    CameraParamsStore() = default;

    QString filePath(const QString& modelName) const;
};

#endif // CAMERAPARAMSSTORE_H
