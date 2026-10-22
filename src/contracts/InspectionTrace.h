#pragma once
#include <QJsonObject>
#include <QString>
#include <QtGlobal>

struct InspectionTrace {
    QString inspectionId;
    quint64 frameId { 0 }, schemeGeneration { 0 };
    qint64 acquiredAtNs { 0 }, acceptedAtNs { 0 }, workerStartedAtNs { 0 };
    QJsonObject toJson() const
    {
        return {{"inspectionId", inspectionId}, {"frameId", QString::number(frameId)},
            {"schemeGeneration", QString::number(schemeGeneration)},
            {"acquiredAtNs", QString::number(acquiredAtNs)},
            {"acceptedAtNs", QString::number(acceptedAtNs)},
            {"workerStartedAtNs", QString::number(workerStartedAtNs)}};
    }
    static InspectionTrace fromJson(const QJsonObject& value)
    {
        return {value["inspectionId"].toString(), value["frameId"].toString().toULongLong(),
            value["schemeGeneration"].toString().toULongLong(), value["acquiredAtNs"].toString().toLongLong(),
            value["acceptedAtNs"].toString().toLongLong(), value["workerStartedAtNs"].toString().toLongLong()};
    }
};
