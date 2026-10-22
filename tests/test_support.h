#pragma once
#include <QCoreApplication>
#include <QElapsedTimer>
#include <QThread>
#include <cstdio>
#include <functional>
struct Checks {
    int failures {0};
    void operator()(bool ok, const char* label) {
        std::printf("[%s] %s\n", ok ? "PASS" : "FAIL", label);
        failures += !ok;
    }
    int result() const { return failures ? 1 : 0; }
};
inline bool until(std::function<bool()> ready, int ms = 2000) {
    QElapsedTimer elapsed; elapsed.start();
    while (!ready() && elapsed.elapsed() < ms) {
        QCoreApplication::processEvents(); QThread::msleep(1);
    }
    return ready();
}
