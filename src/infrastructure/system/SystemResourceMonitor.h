#pragma once

#include <QtGlobal>

struct SystemResourceUsage {
    double cpuPercent { -1.0 };
    double memoryPercent { -1.0 };
    quint64 memoryUsedKiB { 0 };
    quint64 memoryTotalKiB { 0 };
};

class SystemResourceMonitor final {
public:
    SystemResourceUsage sample();
    void reset();

private:
    bool _hasPreviousCpu { false };
    quint64 _previousTotal { 0 };
    quint64 _previousIdle { 0 };
};
