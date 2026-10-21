#pragma once
#include "application/operators/OperatorRegistry.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStringList>

namespace InspectionPresentation {
inline QString input(const QJsonObject& tool)
{
    const auto data = tool.value("input").toObject();
    if (data.isEmpty())
        return QStringLiteral("旧记录未保存输入");
    QString text = data.value("source").toString();
    if (data.contains("width"))
        text += QStringLiteral(" · %1×%2").arg(data.value("width").toInt()).arg(data.value("height").toInt());
    const auto roi = data.value("roi").toArray();
    if (roi.size() == 4)
        text += QStringLiteral(" · ROI (%1,%2,%3,%4)")
                    .arg(roi[0].toDouble())
                    .arg(roi[1].toDouble())
                    .arg(roi[2].toDouble())
                    .arg(roi[3].toDouble());
    return text;
}
inline QString output(const QJsonObject& tool)
{
    const auto fields = tool.value("fields").toObject();
    QStringList parts;
    if (const auto* d = OperatorRegistry::find(tool.value("kind").toString())) {
        for (const auto& field : d->outputs) {
            if (!fields.contains(field.key))
                continue;
            const auto value = fields[field.key];
            const auto text = value.isString() ? value.toString() : QString::number(value.toDouble(), 'g', 6);
            parts << field.label + QStringLiteral("：") + text + field.unit;
        }
    }
    for (const auto& value : fields.value("judgements").toArray()) {
        const auto judgement = value.toObject();
        if (judgement["status"] != "OK")
            parts << judgement["field"].toString() + QStringLiteral("：")
                    + (judgement["error"].toString().isEmpty() ? judgement["reason"].toString()
                                                               : judgement["error"].toString());
    }
    if (!fields.value("message").toString().isEmpty())
        parts << fields.value("message").toString();
    const QString error = tool.value("error").toString();
    if (!error.isEmpty())
        parts << error;
    return parts.isEmpty() ? QStringLiteral("—") : parts.join(QStringLiteral("；"));
}
}
