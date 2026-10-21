#include "TcpModule.h"
#include "infrastructure/communication/TcpServerService.h"
#include "infrastructure/communication/TcpResultPublisher.h"
#include <QObject>
CommunicationModule tcpModule()
{
    CommunicationModule m;
    m.id = "tcp_server";
    m.name = "TCP Server";
    m.listener = true;
    m.description = "向已连接的电脑发送检测结果，每行一条 JSON。";
    m.parameters = { numberParameter("port", "监听端口", 9000, 1, 65535) };
    m.start = [](const QJsonObject& p, QString* error) {
        const bool ok = TcpServerService::instance().start(quint16(p["port"].toInt()));
        if (!ok && error)
            *error = "TCP 监听失败，请检查端口占用";
        return ok;
    };
    m.stop = [] { TcpServerService::instance().stop(); };
    m.status = [] {
        auto& s = TcpServerService::instance();
        return s.isRunning() ? QStringLiteral("监听 %1 · %2 个连接").arg(s.port()).arg(s.clientCount())
                             : QStringLiteral("监听未启动");
    };
    m.publish = [](const QJsonObject& result, QString* error) {
        return TcpResultPublisher().publish(result, error);
    };
    m.observe = [](QObject* owner, std::function<void()> changed, std::function<void(QString)> error) {
        auto& s = TcpServerService::instance();
        QObject::connect(&s, &TcpServerService::clientCountChanged, owner, [changed] { changed(); });
        QObject::connect(&s, &TcpServerService::errorOccurred, owner, error);
    };
    return m;
}
