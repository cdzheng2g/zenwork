#pragma once
#include <QJsonObject>
#include <QString>

class TcpResultPublisher final {
public:
    bool publish(const QJsonObject& result, QString* error = nullptr) const;
};
