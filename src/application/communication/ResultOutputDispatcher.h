#pragma once

#include <QJsonObject>
#include <QList>
#include <QStringList>

struct OutputChannelDescriptor {
    QString id;
    QString name;
};

class ResultOutputDispatcher final {
public:
    static const QList<OutputChannelDescriptor>& channels();
    static QStringList publish(const QStringList& enabledChannels, const QJsonObject& result);
};
