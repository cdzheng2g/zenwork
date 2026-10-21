#include "DetectionDecoderRegistry.h"
#include <stdexcept>

const DetectionDecoderDefinition* DetectionDecoderRegistry::find(const QString& id)
{
    for (const auto& definition : definitions())
        if (definition.id == id)
            return &definition;
    return nullptr;
}

QStringList DetectionDecoderRegistry::ids()
{
    QStringList result;
    for (const auto& definition : definitions()) result.append(definition.id);
    return result;
}

ParameterSchema DetectionDecoderRegistry::configurationSchema()
{
    ParameterSchema result;
    for (const auto& definition : definitions()) {
        for (auto parameter : definition.parameters) {
            parameter.key = "decoder." + definition.id + "." + parameter.key;
            parameter.label = definition.name + " · " + parameter.label;
            parameter.advanced = true;
            result.append(parameter);
        }
    }
    return result;
}

QJsonObject DetectionDecoderRegistry::configuration(const QString& id, const QJsonObject& stored)
{
    const auto* definition = find(id);
    if (!definition) throw std::runtime_error(("未注册的检测解码器：" + id).toStdString());
    QJsonObject result;
    for (const auto& parameter : definition->parameters) {
        const auto key = "decoder." + id + "." + parameter.key;
        result[parameter.key] = stored.value(key).isUndefined() ? parameter.defaultValue : stored[key];
    }
    return result;
}

std::unique_ptr<DetectionDecoder> DetectionDecoderRegistry::create(
    const QString& id,
    const QJsonObject& parameters)
{
    for(const auto& definition: definitions()){
        if (definition.id!=id)
            continue;
        
        if(!definition.factory){
           throw std::runtime_error("解码器没有配置创建函数");
        }
        const auto values = parameterDefaults(definition.parameters, parameters);
        const auto error = validateParameters(definition.parameters, values);
        if (!error.isEmpty()) throw std::runtime_error(error.toStdString());
        auto decoder = definition.factory(values);
        if (!decoder) {
            throw std::runtime_error(
                "解码器创建失败");
        }
        return decoder;
    }
    throw std::runtime_error(
        QStringLiteral("未注册的检测解码器：%1")
            .arg(id)
            .toStdString());
}
