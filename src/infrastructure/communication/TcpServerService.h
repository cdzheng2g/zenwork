#ifndef TCPSERVERSERVICE_H
#define TCPSERVERSERVICE_H

#include <QList>
#include <QObject>
#include <QJsonObject>

class QTcpServer;
class QTcpSocket;

// Broadcast one UTF-8 JSON object per line to connected clients.
class TcpServerService : public QObject {
    Q_OBJECT
public:
    static TcpServerService& instance();

    bool start(quint16 port);
    void stop();
    bool isRunning() const;
    quint16 port() const;
    int clientCount() const;

    void broadcastJson(const QJsonObject& object);

Q_SIGNALS:
    void clientCountChanged(int count);
    void errorOccurred(const QString& message);

private Q_SLOTS:
    void onNewConnection();
    void onClientDisconnected();

private:
    explicit TcpServerService(QObject* parent = nullptr);

    QTcpServer* _server { nullptr };
    QList<QTcpSocket*> _clients;
};

#endif // TCPSERVERSERVICE_H
