#pragma once
#include "ModelSessionCache.h"
#include "RknnSession.h"
inline std::shared_ptr<RknnSession> cachedRknnSession(const QString& path)
{
    thread_local ModelSessionCache<RknnSession> cache;
    return cache.acquire(path);
}
