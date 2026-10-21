#pragma once

#include "DetectionDecoder.h"
#include "contracts/ParameterSpec.h"

#include <QJsonObject>
#include <QList>
#include <QString>
#include <memory>

using DetectionDecoderFactory =
    std::unique_ptr<DetectionDecoder> (*)(const QJsonObject& parameters);

struct DetectionDecoderDefinition
{
    QString id;
    DetectionDecoderFactory factory{nullptr};
    QString name;
    ParameterSchema parameters;
};

class DetectionDecoderRegistry
{
public:
    static const QList<DetectionDecoderDefinition>& definitions();
    static const DetectionDecoderDefinition* find(const QString& id);
    static QStringList ids();

    // Prefix decoder-owned fields so different decoders keep independent settings.
    static ParameterSchema configurationSchema();
    static QJsonObject configuration(const QString& id, const QJsonObject& stored);

    static std::unique_ptr<DetectionDecoder> create(
        const QString& id,
        const QJsonObject& parameters);
};
