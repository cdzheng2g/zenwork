// 相机采集租约的无硬件单元测试。多个页面同时需要画面时，只有首个
// acquire 应启动采集；只有最后一个 release 才允许停止采集。

#include "infrastructure/camera/CameraStreamLease.h"

#include <cstdio>

namespace {
int gFailureCount = 0;

void checkTrue(bool condition, const char* description)
{
    std::printf("[%s] %s\n", condition ? "PASS" : "FAIL", description);
    if (!condition)
        ++gFailureCount;
}
} // namespace

int main()
{
    CameraStreamLease lease;

    checkTrue(lease.acquire(), "first consumer starts capture");
    checkTrue(!lease.acquire(), "second consumer reuses running capture");
    checkTrue(lease.consumerCount() == 2, "two consumers are tracked");
    checkTrue(!lease.release(), "releasing one consumer preserves capture");
    checkTrue(lease.consumerCount() == 1, "one consumer remains");
    checkTrue(lease.release(), "last consumer stops capture");
    checkTrue(lease.consumerCount() == 0, "all consumers are released");
    checkTrue(!lease.release(), "extra release does not request another stop");

    return gFailureCount == 0 ? 0 : 1;
}
