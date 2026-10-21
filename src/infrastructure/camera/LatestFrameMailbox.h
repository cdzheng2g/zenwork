#pragma once
#include <QMutex>
#include <QMutexLocker>
#include <opencv2/core.hpp>

// At most one queued delivery and one retained camera frame, even if UI is busy.
class LatestFrameMailbox {
public:
    bool offer(const cv::Mat& frame)
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
        QMutexLocker lock(&_mutex);
        cv::Mat result = std::move(_latest);
        _pending = false;
        return result;
    }

private:
    QMutex _mutex;
    cv::Mat _latest;
    bool _pending { false };
};
