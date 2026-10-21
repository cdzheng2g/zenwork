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
        client->disconnectFromHost();
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

void TcpServerService::broadcastJson(const QJsonObject& object)
{
    const QByteArray payload = QJsonDocument(object).toJson(QJsonDocument::Compact) + '\n';
    for (QTcpSocket* client : _clients)
        client->write(payload);
}

void TcpServerService::onNewConnection()
{
    while (_server->hasPendingConnections()) {
        QTcpSocket* client = _server->nextPendingConnection();
        connect(client, &QTcpSocket::disconnected, this, &TcpServerService::onClientDisconnected);
        _clients.append(client);
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
