#include "infrastructure/camera/CameraStreamLease.h"

bool CameraStreamLease::acquire()
{
    const bool shouldStart = _consumerCount == 0;
    ++_consumerCount;
    return shouldStart;
}

bool CameraStreamLease::release()
{
    if (_consumerCount == 0)
        return false;

    --_consumerCount;
    return _consumerCount == 0;
}

int CameraStreamLease::consumerCount() const
{
    return _consumerCount;
}
