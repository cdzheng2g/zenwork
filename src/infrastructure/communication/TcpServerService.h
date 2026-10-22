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
    static constexpr int MaximumClients = 16;
    static constexpr qint64 MaximumPendingBytes = 1024 * 1024;
    static constexpr qint64 MaximumReadBytes = 4096;

    bool start(quint16 port);
    void stop();
    bool isRunning() const;
    quint16 port() const;
    int clientCount() const;

    bool broadcastJson(const QJsonObject& object, QString* error = nullptr);

Q_SIGNALS:
    void clientCountChanged(int count);
    void errorOccurred(const QString& message);

private Q_SLOTS:
    void onNewConnection();
    void onClientDisconnected();

private:
    explicit TcpServerService(QObject* parent = nullptr);
    void rejectClient(QTcpSocket* client, const QString& reason);

    QTcpServer* _server { nullptr };
    QList<QTcpSocket*> _clients;
};

#endif // TCPSERVERSERVICE_H
