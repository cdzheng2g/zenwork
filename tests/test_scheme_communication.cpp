#include "application/communication/SchemeCommunicationRuntime.h"
#include "infrastructure/communication/ModbusTcpServerService.h"
#include "infrastructure/communication/ModbusTcpOutputService.h"
#include "infrastructure/communication/TcpServerService.h"
#include "application/communication/ResultOutputDispatcher.h"
#include "application/communication/ResultDeliveryTracker.h"
#include <QCoreApplication>
#include <QElapsedTimer>
#include <QModbusTcpServer>
#include <QModbusDataUnit>
#include <QTcpServer>
#include <QTcpSocket>
#include <QThread>
#include <functional>
#include <cstdio>

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

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);
    int failures = 0;
    auto check = [&](bool ok, const char* message) {
        printf("[%s] %s\n", ok ? "PASS" : "FAIL", message);
        if (!ok)
            ++failures;
    };
    QTcpServer first, second, third;
    if (!first.listen(QHostAddress::LocalHost, 0) || !second.listen(QHostAddress::LocalHost, 0)
        || !third.listen(QHostAddress::LocalHost, 0))
        return 1;
    const auto a = first.serverPort(), b = second.serverPort(), c = third.serverPort();
    first.close();
    second.close();
    third.close();
    auto& runtime = SchemeCommunicationRuntime::instance();
    auto& tcp = TcpServerService::instance();
    auto& modbus = ModbusTcpServerService::instance();
    auto& client = ModTcpOutputService::instance();
    SchemeRuntimeSettings settings;
    settings.communication
        = normalizedCommunicationSettings({ { "tcp_server", QJsonObject { { "port", int(a) } } },
            { "modbus_tcp_server", QJsonObject { { "port", int(b) }, { "unitId", 7 } } } });
    settings.outputChannels = QStringList { "tcp_server", "modbus_tcp_server" };
    QString error;
    check(runtime.apply(settings, &error) && tcp.isRunning() && modbus.isRunning() && modbus.port() == b
            && modbus.unitId() == 7,
        "saved scheme automatically starts its listeners");
    QTcpSocket observer;
    observer.connectToHost(QHostAddress::LocalHost, a);
    check(waitFor(
              [&] { return observer.state() == QAbstractSocket::ConnectedState && tcp.clientCount() == 1; }),
        "computer connects to automatic TCP listener");
    check(runtime.apply(settings, &error) && observer.state() == QAbstractSocket::ConnectedState
            && tcp.clientCount() == 1,
        "reapplying an unchanged scheme preserves connected clients");
    auto invalid = settings;
    invalid.communication["modbus_tcp_server"] = QJsonObject { { "port", int(a) }, { "unitId", 1 } };
    check(!runtime.apply(invalid, &error) && !error.isEmpty() && modbus.port() == b,
        "conflicting ports are rejected before disturbing the current scheme");
    settings.communication["modbus_tcp_server"] = QJsonObject { { "port", int(a) }, { "unitId", 3 } };
    settings.communication["tcp_server"] = QJsonObject { { "port", int(b) } };
    check(runtime.apply(settings, &error) && tcp.port() == b && modbus.port() == a && modbus.unitId() == 3,
        "switching schemes can swap listener ports without a bind conflict");
    settings.outputChannels.clear();
    settings.modbusTriggerEnabled = true;
    check(runtime.apply(settings, &error) && !tcp.isRunning() && modbus.isRunning()
            && runtime.usesChannel("modbus_tcp_server"),
        "trigger-only Modbus remains listening with result output disabled");
    settings.modbusTriggerEnabled = false;
    check(runtime.apply(settings, &error) && !modbus.isRunning() && !tcp.isRunning(),
        "a scheme without output or Modbus trigger releases unused services");
    QModbusTcpServer notebook;
    QModbusDataUnitMap map;
    map.insert(QModbusDataUnit::HoldingRegisters, QModbusDataUnit(QModbusDataUnit::HoldingRegisters, 0, 2));
    notebook.setMap(map);
    notebook.setServerAddress(1);
    notebook.setConnectionParameter(QModbusDevice::NetworkAddressParameter, "127.0.0.1");
    notebook.setConnectionParameter(QModbusDevice::NetworkPortParameter, c);
    if (!notebook.connectDevice())
        return 1;
    settings.outputChannels = QStringList { "modbus_tcp_client" };
    settings.communication["modbus_tcp_client"]
        = QJsonObject { { "host", "127.0.0.1" }, { "port", int(c) }, { "unitId", 1 } };
    check(runtime.apply(settings, &error) && waitFor([&] { return client.isConnected(); }),
        "client scheme automatically connects to the configured server");
    quint16 sequence = 0;
    check(client.publish(1, 23, &error) && waitFor([&] {
        notebook.data(QModbusDataUnit::HoldingRegisters, 1, &sequence);
        return sequence == 23;
    }),
        "automatic client connection publishes inspection data");
    const QString fullSequence = "9007199254740993";
    const auto deliveryErrors = ResultOutputDispatcher::publish({"modbus_tcp_client"},
        {{"inspectionId", "protocol-ack-test"}, {"sequenceText", fullSequence},
            {"sequence", 24}, {"status", "OK"}});
    auto& tracker = ResultDeliveryTracker::instance();
    check(deliveryErrors.isEmpty() && waitFor([&] {
        return tracker.latest("protocol-ack-test", "modbus_tcp_client").state == DeliveryState::Confirmed;
    }) && tracker.latest("protocol-ack-test", "modbus_tcp_client").sequenceText == fullSequence,
        "real local Modbus reply confirms its originating UUID and full precision sequence");
    runtime.stopAll();
    check(waitFor([&] { return !client.isConnected(); }) && !modbus.isRunning() && !tcp.isRunning(),
        "removing the last scheme stops its services");
    if (!first.listen(QHostAddress::Any, 0))
        return 1;
    settings.outputChannels = QStringList { "tcp_server" };
    settings.communication["tcp_server"] = QJsonObject { { "port", int(first.serverPort()) } };
    check(!runtime.apply(settings, &error) && !error.isEmpty() && !tcp.isRunning(),
        "occupied ports produce a visible error instead of claiming success");
    runtime.stopAll();
    return failures ? 1 : 0;
}
