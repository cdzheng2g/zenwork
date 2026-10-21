#include "YamlDocument.h"
#include <QFile>
#include <QJsonArray>
#include <QSaveFile>
#include <QRegularExpression>
#include <yaml-cpp/yaml.h>
#include <cmath>
#include <stdexcept>

namespace {
void writeValue(YAML::Emitter& out, const QJsonValue& value)
{
    if (value.isObject()) {
        out << YAML::BeginMap;
        const auto object = value.toObject();
        for (auto it = object.begin(); it != object.end(); ++it) {
            out << YAML::Key << it.key().toStdString() << YAML::Value;
            writeValue(out, it.value());
        }
        out << YAML::EndMap;
    } else if (value.isArray()) {
        out << YAML::BeginSeq;
        for (const auto& item : value.toArray())
            writeValue(out, item);
        out << YAML::EndSeq;
    } else if (value.isString())
        out << YAML::DoubleQuoted << value.toString().toStdString();
    else if (value.isBool())
        out << value.toBool();
    else if (value.isDouble())
        out << value.toDouble();
    else
        out << YAML::Null;
}
QJsonValue convert(const YAML::Node& node, int depth, int& count)
{
    if (depth > 24 || ++count > 100000)
        throw std::runtime_error("YAML 结构过深或节点过多");
    if (node.IsMap()) {
        QJsonObject object;
        for (const auto& item : node) {
            const QString key = QString::fromStdString(item.first.as<std::string>());
            if (object.contains(key))
                throw std::runtime_error("YAML 存在重复字段");
            object[key] = convert(item.second, depth + 1, count);
        }
        return object;
    }
    if (node.IsSequence()) {
        QJsonArray array;
        for (const auto& item : node)
            array.append(convert(item, depth + 1, count));
        return array;
    }
    if (node.IsNull())
        return QJsonValue();
    const QString value = QString::fromStdString(node.Scalar());
    if (node.Tag() == "!" || node.Tag() == "tag:yaml.org,2002:str")
        return value;
    if (value.compare("true", Qt::CaseInsensitive) == 0)
        return true;
    if (value.compare("false", Qt::CaseInsensitive) == 0)
        return false;
    static const QRegularExpression number(
        QStringLiteral("^[+-]?(?:[0-9]+(?:\\.[0-9]*)?|\\.[0-9]+)(?:[eE][+-]?[0-9]+)?$"));
    bool ok = false;
    const double numeric = value.toDouble(&ok);
    if (ok && std::isfinite(numeric) && number.match(value).hasMatch())
        return numeric;
    return value;
}
}
bool YamlDocument::read(const QString& path, QJsonObject& object, QString* error)
{
    try {
        QFile file(path);
        if (!file.open(QIODevice::ReadOnly))
            throw std::runtime_error(file.errorString().toStdString());
        if (file.size() > 256 * 1024 * 1024)
            throw std::runtime_error("YAML 文件超过 256 MB");
        const auto documents = YAML::LoadAll(file.readAll().toStdString());
        if (documents.size() != 1 || !documents.front().IsMap())
            throw std::runtime_error("需要一个 YAML 映射文档");
        int count = 0;
        object = convert(documents.front(), 0, count).toObject();
        return true;
    } catch (const std::exception& e) {
        if (error)
            *error = QString::fromUtf8(e.what());
        return false;
    }
}
bool YamlDocument::write(const QString& path, const QJsonObject& object, QString* error)
{
    try {
        YAML::Emitter out;
        out.SetDoublePrecision(17);
        writeValue(out, object);
        if (!out.good())
            throw std::runtime_error(out.GetLastError());
        const QByteArray bytes = QByteArray("# Hans AI Camera portable YAML\n") + out.c_str() + '\n';
        if (bytes.size() > 256 * 1024 * 1024)
            throw std::runtime_error("方案包含的资源超过 256 MB");
        QSaveFile file(path);
        if (!file.open(QIODevice::WriteOnly) || file.write(bytes) != bytes.size() || !file.commit())
            throw std::runtime_error(file.errorString().toStdString());
        return true;
    } catch (const std::exception& e) {
        if (error)
            *error = QString::fromUtf8(e.what());
        return false;
    }
}
