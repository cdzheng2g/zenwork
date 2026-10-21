#pragma once
#include "infrastructure/camera/CameraParams.h"
#include <QJsonObject>
inline QJsonObject cameraParamsJson(const CameraParams& p)
{
    return { { "autoExposure", p.autoExposure }, { "exposureMs", p.exposureMs }, { "gain", p.gain },
        { "ispDGain", p.ispDGain }, { "contrast", p.contrast }, { "saturation", p.saturation },
        { "sharpness", p.sharpness } };
}
inline CameraParams cameraParamsFromJson(const QJsonObject& o)
{
    CameraParams p;
    p.autoExposure = o.value("autoExposure").toBool(true);
    p.exposureMs = o.value("exposureMs").toDouble(10);
    p.gain = o.value("gain").toDouble(1);
    p.ispDGain = o.value("ispDGain").toDouble(1);
    p.contrast = o.value("contrast").toInt(128);
    p.saturation = o.value("saturation").toInt(128);
    p.sharpness = o.value("sharpness").toDouble(100);
    return p;
}
