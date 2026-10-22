#include "infrastructure/communication/TcpServerService.h"

#include <QHostAddress>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTcpServer>
#include <QTcpSocket>

TcpServerService& TcpServerService::instance()
{
    static TcpServerService instance;
    return instance;
}

TcpServerService::TcpServerService(QObject* parent)
    : QObject(parent)
{
    _server = new QTcpServer(this);
    connect(_server, &QTcpServer::newConnection, this, &TcpServerService::onNewConnection);
}

bool TcpServerService::start(quint16 port)
{
    if (_server->isListening()) {
        if (_server->serverPort() == port)
            return true;
        stop();
    }

    if (!_server->listen(QHostAddress::Any, port)) {
        Q_EMIT errorOccurred(_server->errorString());
        return false;
    }
    return true;
}

void TcpServerService::stop()
{
    const auto clients = _clients;
    for (QTcpSocket* client : clients)
        client->abort();
    _clients.clear();
    _server->close();
    Q_EMIT clientCountChanged(0);
}

bool TcpServerService::isRunning() const
{
    return _server->isListening();
}

quint16 TcpServerService::port() const
{
    return _server->serverPort();
}

int TcpServerService::clientCount() const
{
    return _clients.size();
}

bool TcpServerService::broadcastJson(const QJsonObject& object, QString* error)
{
    if (error) error->clear();
    const QByteArray payload = QJsonDocument(object).toJson(QJsonDocument::Compact) + '\n';
    if (payload.size() > MaximumPendingBytes) {
        Q_EMIT errorOccurred(QStringLiteral("TCP 结果超过单条发送上限，未发送"));
        if (error) *error = QStringLiteral("TCP 结果超过单条发送上限");
        return false;
    }
    // abort() may synchronously remove a client through disconnected().
    const auto clients = _clients;
    bool sent = false, failed = false;
    for (QTcpSocket* client : clients) {
        if (client->state() != QAbstractSocket::ConnectedState)
            continue;
        if (client->bytesToWrite() > MaximumPendingBytes - payload.size()) {
            rejectClient(client, QStringLiteral("TCP 客户端发送积压超过上限，已断开"));
            failed = true;
            continue;
        }
        if (client->write(payload) != payload.size()) {
            failed = true;
            rejectClient(client, QStringLiteral("TCP 结果提交不完整，已断开客户端"));
        } else sent = true;
    }
    if ((!sent || failed) && error) *error = failed
        ? QStringLiteral("部分或全部 TCP 客户端未能提交结果") : QStringLiteral("没有可接收结果的 TCP 客户端");
    return sent && !failed;
}

void TcpServerService::rejectClient(QTcpSocket* client, const QString& reason)
{
    // Do not wait for a slow peer to drain pending output during rejection.
    client->abort();
    Q_EMIT errorOccurred(reason);
}

void TcpServerService::onNewConnection()
{
    while (_server->hasPendingConnections()) {
        QTcpSocket* client = _server->nextPendingConnection();
        if (!client)
            continue;
        if (_clients.size() >= MaximumClients) {
            client->abort();
            client->deleteLater();
            Q_EMIT errorOccurred(QStringLiteral("TCP 已达到最大客户端数量，拒绝新连接"));
            continue;
        }
        client->setReadBufferSize(MaximumReadBytes);
        connect(client, &QTcpSocket::disconnected, this, &TcpServerService::onClientDisconnected);
        _clients.append(client);
        // This endpoint only publishes JSON lines; inbound application data is invalid.
        connect(client, &QTcpSocket::readyRead, this, [this, client] {
            rejectClient(client, QStringLiteral("TCP 结果通道不接受输入数据，已断开客户端"));
        });
        if (client->bytesAvailable() > 0)
            rejectClient(client, QStringLiteral("TCP 结果通道不接受输入数据，已断开客户端"));
    }
    Q_EMIT clientCountChanged(_clients.size());
}

void TcpServerService::onClientDisconnected()
{
    QTcpSocket* client = qobject_cast<QTcpSocket*>(sender());
    if (!client)
        return;
    _clients.removeAll(client);
    client->deleteLater();
    Q_EMIT clientCountChanged(_clients.size());
}
