#pragma once
#include "SchemeCatalog.h"
#include "ToolCodec.h"
#include "infrastructure/communication/LegacyCommunicationSettings.h"
#include <QSet>
namespace SchemeCodec {
inline QJsonObject encodeSettings(const SchemeRuntimeSettings& s)
{
    return { { "internalTriggerEnabled", s.internalTriggerEnabled },
        { "modbusTriggerEnabled", s.modbusTriggerEnabled }, { "triggerIntervalMs", s.triggerIntervalMs },
        { "logicOr", s.logicOr }, { "referenceModel", s.referenceModel }, { "camera", s.camera },
        { "communication", s.communication },
        { "outputChannels", QJsonArray::fromStringList(s.outputChannels) } };
}
inline SchemeRuntimeSettings decodeSettings(const QJsonObject& o, bool migrateLocalCommunication = false)
{
    SchemeRuntimeSettings s;
    for (const auto& key : { "internalTriggerEnabled", "modbusTriggerEnabled", "logicOr" })
        if (o.contains(key) && !o[key].isBool())
            throw std::runtime_error("方案开关字段类型无效");
    if (o.contains("triggerIntervalMs")
        && (!o["triggerIntervalMs"].isDouble() || o["triggerIntervalMs"].toDouble() < 50
            || o["triggerIntervalMs"].toDouble() > 60000
            || o["triggerIntervalMs"].toDouble() != std::floor(o["triggerIntervalMs"].toDouble())))
        throw std::runtime_error("触发周期必须为整数");
    for (const auto& key : { "camera", "communication" })
        if (o.contains(key) && !o[key].isObject())
            throw std::runtime_error("方案参数格式无效");
    s.internalTriggerEnabled = o["internalTriggerEnabled"].toBool(true);
    s.modbusTriggerEnabled = o["modbusTriggerEnabled"].toBool(false);
    s.triggerIntervalMs = o["triggerIntervalMs"].toInt(500);
    s.logicOr = o["logicOr"].toBool(false);
    s.referenceModel = o["referenceModel"].toString();
    s.camera = o["camera"].toObject();
    s.communication = (!o.contains("communication") && migrateLocalCommunication)
        ? legacyCommunicationSettings()
        : o["communication"].toObject();
    if (o.contains("outputChannels")) {
        if (!o["outputChannels"].isArray())
            throw std::runtime_error("输出通道格式无效");
        s.outputChannels.clear();
        for (const auto& v : o["outputChannels"].toArray()) {
            if (!v.isString())
                throw std::runtime_error("输出通道必须是字符串");
            s.outputChannels.append(v.toString());
        }
    }
    return s;
}
inline QString validate(const SchemeRuntimeSettings& s, const QList<SchemeTool>& tools)
{
    if (s.triggerIntervalMs < 50 || s.triggerIntervalMs > 60000)
        return "触发周期必须为 50–60000 ms";
    if (s.internalTriggerEnabled && s.modbusTriggerEnabled)
        return "内部触发与 Modbus 触发不能同时启用";
    const auto invalid
        = validateCommunicationSettings(s.communication, s.outputChannels, s.modbusTriggerEnabled);
    if (!invalid.isEmpty())
        return invalid;
    QSet<QString> ids, kinds;
    for (const auto& tool : tools) {
        auto error = OperatorRegistry::validate(tool);
        if (!error.isEmpty())
            return error;
        if (ids.contains(tool.id))
            return "算子 ID 重复";
        if (kinds.contains(tool.kind) && !OperatorRegistry::find(tool.kind)->allowsMultiple)
            return "当前算子只能配置一个实例";
        ids.insert(tool.id);
        kinds.insert(tool.kind);
    }
    return { };
}
inline QJsonArray encodeTools(const QList<SchemeTool>& tools)
{
    QJsonArray array;
    for (const auto& tool : tools)
        array.append(ToolCodec::encode(tool));
    return array;
}
inline QList<SchemeTool> decodeTools(const QJsonArray& array, const QJsonObject& legacyOcr = { })
{
    QList<SchemeTool> tools;
    for (const auto& value : array) {
        if (!value.isObject())
            throw std::runtime_error("算子必须是对象");
        tools.append(ToolCodec::decode(value.toObject(), legacyOcr));
    }
    return tools;
}
}
