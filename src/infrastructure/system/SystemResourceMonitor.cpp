#include "infrastructure/system/SystemResourceMonitor.h"

#include <QFile>

namespace {
bool readCpuTotals(quint64& total, quint64& idle)
{
    QFile file(QStringLiteral("/proc/stat"));
    if (!file.open(QIODevice::ReadOnly))
        return false;
    const auto columns = file.readLine().simplified().split(' ');
    if (columns.size() < 9 || columns.first() != "cpu")
        return false;

    quint64 values[8] { };
    for (int i = 0; i < 8; ++i) {
        bool ok = false;
        values[i] = columns.at(i + 1).toULongLong(&ok);
        if (!ok)
            return false;
    }
    total = 0;
    for (const auto value : values)
        total += value;
    idle = values[3] + values[4];
    return true;
}

bool readMemory(quint64& total, quint64& available)
{
    QFile file(QStringLiteral("/proc/meminfo"));
    if (!file.open(QIODevice::ReadOnly))
        return false;
    total = 0;
    available = 0;
    bool foundTotal = false;
    bool foundAvailable = false;
    for (;;) {
        const QByteArray line = file.readLine();
        if (line.isEmpty())
            break;
        const auto columns = line.simplified().split(' ');
        if (columns.size() < 2)
            continue;
        bool ok = false;
        const quint64 value = columns.at(1).toULongLong(&ok);
        if (!ok)
            continue;

        if (columns.at(0) == "MemTotal:") {
            total = value;
            foundTotal = true;
        }
        if (columns.at(0) == "MemAvailable:") {
            available = value;
            foundAvailable = true;
        }
    }
    return foundTotal && foundAvailable && available <= total;
}

}

SystemResourceUsage SystemResourceMonitor::sample()
{
    SystemResourceUsage result;
    quint64 total = 0;
    quint64 idle = 0;
    if (readCpuTotals(total, idle)) {
        if (_hasPreviousCpu && total > _previousTotal && idle >= _previousIdle) {
            const quint64 totalDelta = total - _previousTotal;
            const quint64 idleDelta = idle - _previousIdle;
            result.cpuPercent = 100.0 * double(totalDelta - qMin(totalDelta, idleDelta)) / double(totalDelta);
        }
        _previousTotal = total;
        _previousIdle = idle;
        _hasPreviousCpu = true;
    } else {
        _hasPreviousCpu = false;
    }

    quint64 available = 0;
    if (readMemory(total, available)) {
        result.memoryTotalKiB = total;
        result.memoryUsedKiB = total - available;
        result.memoryPercent = 100.0 * double(result.memoryUsedKiB) / double(result.memoryTotalKiB);
    }
    return result;
}

void SystemResourceMonitor::reset()
{
    _hasPreviousCpu = false;
    _previousTotal = 0;
    _previousIdle = 0;
}
