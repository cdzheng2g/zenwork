#include "infrastructure/communication/TcpServerService.h"
#include <QCoreApplication>
#include <QElapsedTimer>
#include <QEventLoop>
#include <QHostAddress>
#include <QTcpSocket>
#include <QThread>
#include <cstdio>
#include <functional>
#include <memory>
#include <vector>

bool waitFor(const std::function<bool()>& done)
{
    QElapsedTimer timer;
    timer.start();
    while (!done() && timer.elapsed() < 3000) {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 5);
        QThread::msleep(1);
    }
    return done();
}

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);
    auto& server = TcpServerService::instance();
    int failures = 0, errors = 0;
    auto check = [&](bool ok, const char* label) {
        std::printf("[%s] %s\n", ok ? "PASS" : "FAIL", label);
        failures += !ok;
    };
    QObject::connect(&server, &TcpServerService::errorOccurred, &app, [&](const QString&) { ++errors; });
    check(server.start(0), "start an ephemeral loopback test endpoint");
    const auto port = server.port();
    auto connectClient = [&] {
        auto client = std::make_unique<QTcpSocket>();
        client->connectToHost(QHostAddress::LocalHost, port);
        return client;
    };
    auto observer = connectClient();
    check(waitFor([&] { return server.clientCount() == 1; }), "client is accepted");
    server.broadcastJson({{"status", "OK"}});
    check(waitFor([&] { return observer->canReadLine(); })
            && observer->readLine() == "{\"status\":\"OK\"}\n",
        "normal result retains the existing JSON-line protocol");

    const int beforeOversize = errors;
    server.broadcastJson({{"data", QString(int(TcpServerService::MaximumPendingBytes), 'x')}});
    check(errors == beforeOversize + 1 && server.clientCount() == 1,
        "oversized result is rejected without queuing it or dropping a healthy client");
    observer->write("unexpected client input");
    check(waitFor([&] { return server.clientCount() == 0
                && observer->state() == QAbstractSocket::UnconnectedState; }),
        "publish-only endpoint rejects inbound application data");

    std::vector<std::unique_ptr<QTcpSocket>> clients;
    for (int i = 0; i < TcpServerService::MaximumClients; ++i) {
        clients.push_back(connectClient());
        check(waitFor([&] { return server.clientCount() == i + 1; }), "accept client within the connection limit");
    }
    const int beforeOverflow = errors;
    auto overflow = connectClient();
    check(waitFor([&] { return errors > beforeOverflow
                && overflow->state() == QAbstractSocket::UnconnectedState; })
            && server.clientCount() == TcpServerService::MaximumClients,
        "excess connection is rejected without removing existing clients");
    clients.front()->abort();
    check(waitFor([&] { return server.clientCount() == TcpServerService::MaximumClients - 1; }),
        "disconnect releases a connection slot");
    auto replacement = connectClient();
    check(waitFor([&] { return server.clientCount() == TcpServerService::MaximumClients; }),
        "released connection slot is reusable");
    server.stop();
    clients.clear();
    check(!server.isRunning() && server.clientCount() == 0, "stop immediately releases accepted clients");

    check(server.start(port), "restart the listener after bounded shutdown");
    auto slow = connectClient();
    check(waitFor([&] { return server.clientCount() == 1; }), "slow test client is accepted");
    const QJsonObject large {{"data", QString(700 * 1024, 'x')}};
    // No event processing between writes: deterministically exercise application backlog.
    server.broadcastJson(large);
    const int beforeBacklog = errors;
    server.broadcastJson(large);
    check(waitFor([&] { return server.clientCount() == 0; }) && errors > beforeBacklog,
        "pending output cannot grow past the per-client high-water mark");
    server.stop();
    return failures ? 1 : 0;
}
