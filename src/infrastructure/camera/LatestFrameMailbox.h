#pragma once
#include <QMutex>
#include <QMutexLocker>
#include <opencv2/core.hpp>
#include "contracts/FrameEnvelope.h"

// At most one queued delivery and one retained camera frame, even if UI is busy.
class LatestFrameMailbox {
public:
    bool offer(const cv::Mat& frame)
    {
        return offerFrame({frame, 0, monotonicNowNs()});
    }
    bool offerFrame(const FrameEnvelope& frame)
    {
        QMutexLocker lock(&_mutex);
        _latest = frame;
        if (_pending)
            return false;
        _pending = true;
        return true;
    }
    cv::Mat take()
    {
        return takeFrame().image;
    }
    FrameEnvelope takeFrame()
    {
        QMutexLocker lock(&_mutex);
        FrameEnvelope result = std::move(_latest);
        _pending = false;
        return result;
    }

private:
    QMutex _mutex;
    FrameEnvelope _latest;
    bool _pending { false };
};
