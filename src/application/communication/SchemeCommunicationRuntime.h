#pragma once

#include "application/scheme/SchemeCatalog.h"
#include <QObject>
#include <QMap>

class SchemeCommunicationRuntime final : public QObject {
    Q_OBJECT
public:
    static SchemeCommunicationRuntime& instance();
    bool apply(const SchemeRuntimeSettings& settings, QString* error = nullptr);
    void stopAll();
    bool usesChannel(const QString& id) const;
    QJsonObject configuration(const QString& id) const;
    QString status(const QString& id) const;
    QString summary() const;
signals:
    void statusChanged();

private:
    explicit SchemeCommunicationRuntime(QObject* parent);
    QJsonObject _configuration;
    QStringList _channels;
    QMap<QString, QString> _errors;
};
