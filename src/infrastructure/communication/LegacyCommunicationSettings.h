#pragma once
#include "application/scheme/CommunicationSettings.h"
#include <QSettings>
#include <QStandardPaths>

// Called only for schemes saved before communication parameters became part of the scheme.
inline QJsonObject legacyCommunicationSettings()
{
    const QString dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QSettings tcp(dir + "/communication.ini", QSettings::IniFormat);
    QSettings client(dir + "/communication_modbus_client.ini", QSettings::IniFormat);
    QSettings server(dir + "/modbus_tcp_server.ini", QSettings::IniFormat);
    return { { "tcp_server",
                 QJsonObject { { "port", qBound(1, tcp.value("tcp/port", 9000).toInt(), 65535) } } },
        { "modbus_tcp_client",
            QJsonObject { { "host", client.value("client/host").toString() },
                { "port", qBound(1, client.value("client/port", 502).toInt(), 65535) },
                { "unitId", qBound(1, client.value("client/unitId", 1).toInt(), 247) } } },
        { "modbus_tcp_server",
            QJsonObject { { "port", qBound(1, server.value("modbus/port", 1502).toInt(), 65535) },
                { "unitId", qBound(1, server.value("modbus/unitId", 1).toInt(), 247) } } } };
}
