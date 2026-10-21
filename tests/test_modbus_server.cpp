#include "infrastructure/communication/ModbusTcpServerService.h"

#include <QCoreApplication>
#include <QElapsedTimer>
#include <QEventLoop>
#include <QModbusTcpClient>
#include <QModbusReply>
#include <QTcpServer>
#include <QThread>
#include <functional>
#include <cstdio>

namespace {
bool waitFor(const std::function<bool()>& done)
{
    QElapsedTimer timer;
    timer.start();
    while (!done() && timer.elapsed() < 3000) {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 10);
        QThread::msleep(1);
    }
    return done();
}
}

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);
    int failures = 0;
    auto check = [&](bool ok, const char* name) {
        std::printf("[%s] %s\n", ok ? "PASS" : "FAIL", name);
        if (!ok)
            ++failures;
    };
    QTcpServer probe;
    if (!probe.listen(QHostAddress::LocalHost, 0))
        return 1;
    const quint16 port = probe.serverPort();
    probe.close();
    ModbusTcpServerService service;
    check(service.start(port, 1), "TCP server listens");
    check(service.start(port, 1), "starting an active server is idempotent");
    QModbusTcpClient client;
    client.setConnectionParameter(QModbusDevice::NetworkAddressParameter, "127.0.0.1");
    client.setConnectionParameter(QModbusDevice::NetworkPortParameter, port);
    client.connectDevice();
    if (!waitFor([&] { return client.state() == QModbusDevice::ConnectedState; }))
        return 1;
    auto read = [&]() {
        auto* reply = client.sendReadRequest(QModbusDataUnit(QModbusDataUnit::HoldingRegisters, 768, 12), 1);
        if (!reply || !waitFor([&] { return reply->isFinished(); }))
            return QVector<quint16> { };
        const auto values
            = reply->error() == QModbusDevice::NoError ? reply->result().values() : QVector<quint16> { };
        delete reply;
        return values;
    };
    auto write = [&](int address, quint16 value) {
        auto* reply = client.sendWriteRequest(
            QModbusDataUnit(QModbusDataUnit::HoldingRegisters, address, QVector<quint16> { value }), 1);
        if (!reply || !waitFor([&] { return reply->isFinished(); }))
            return false;
        const bool ok = reply->error() == QModbusDevice::NoError;
        delete reply;
        return ok;
    };
    const auto initial = read();
    check(initial.size() == 12 && initial[0] == 0 && initial[1] == 0 && initial[11] == 1,
        "registers begin with no result and protocol version 1");
    service.setExecutionState(true, false);
    check(read().value(0) == 1, "ready state is readable");
    int triggers = 0;
    QObject::connect(&service, &ModbusTcpServerService::triggerRequested, [&] { ++triggers; });
    check(write(256, 1) && triggers == 1, "rising edge triggers once");
    check(write(256, 1) && triggers == 1, "repeated high value does not trigger twice");
    check(write(256, 3) && triggers == 1, "other bits do not produce a rising edge");
    check(write(256, 0) && triggers == 1 && write(256, 1) && triggers == 2, "zero rearms the next trigger");
    check(!write(769, 99) && read().value(1) == 0, "client cannot overwrite application results");
    check(!write(257, 1) && !write(255, 1), "reserved and unmapped writes are rejected");
    service.setExecutionState(true, true);
    service.respondToTrigger(ModbusRegisters::BusyRejected);
    const auto busy = read();
    check(busy.value(0) == 2 && busy.value(8) == 1 && busy.value(10) == 2,
        "busy state, rejected count and trigger reply are readable");
    QString error;
    check(service.publish({ { "status", "NG" }, { "sequence", 65536.0 }, { "elapsedMs", 70000.0 } }, &error),
        "inspection result updates the register map");
    const auto result = read();
    check(result.value(1) == 2 && result.value(2) == 0 && result.value(3) == 1 && result.value(4) == 4464
            && result.value(5) == 1 && result.value(6) == 1,
        "NG, 32-bit sequence and duration survive the 16-bit boundary");
    service.setExecutionState(false, false);
    service.respondToTrigger(ModbusRegisters::NotReadyRejected);
    check(read().value(0) == 0 && read().value(8) == 2 && read().value(10) == 3,
        "not-ready response counts separately from accepted triggers");
    client.disconnectDevice();
    service.stop();
    check(!service.publish({ }, &error) && !error.isEmpty(), "stopped service reports a publish failure");
    check(service.start(port, 1), "service restarts");
    client.connectDevice();
    check(waitFor([&] { return client.state() == QModbusDevice::ConnectedState; }), "client reconnects");
    check(write(256, 1) && triggers == 3, "restart clears the command bit for the first new trigger");
    check(read().value(6) == 1, "restart preserves result counters within the process");
    return failures ? 1 : 0;
}
