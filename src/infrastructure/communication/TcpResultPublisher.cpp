#include "TcpResultPublisher.h"
#include "infrastructure/communication/TcpServerService.h"

bool TcpResultPublisher::publish(const QJsonObject& result, QString* error) const
{
    auto& server = TcpServerService::instance();
    if (!server.isRunning()) {
        if (error)
            *error = QStringLiteral("TCP Server 未启动");
        return false;
    }
    return server.broadcastJson(result, error);
}
