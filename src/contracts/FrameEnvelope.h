#pragma once
#include <QMetaType>
#include <QtGlobal>
#include "MonotonicClock.h"
#include <opencv2/core.hpp>

struct FrameEnvelope {
    cv::Mat image;
    quint64 id { 0 };
    // Host monotonic time immediately after frame acquisition, before conversion.
    qint64 acquiredAtNs { 0 };
    bool fresh(int maximumAgeMs = 1500) const
    {
        const auto age = monotonicNowNs() - acquiredAtNs;
        return !image.empty() && acquiredAtNs > 0 && age >= 0
            && age < qint64(maximumAgeMs) * 1000000;
    }
};
Q_DECLARE_METATYPE(FrameEnvelope)
