#include "OperatorResources.h"
#include <QCoreApplication>
#include <QDir>
#include <stdexcept>

QString operatorResourcePath(const QJsonObject& parameters, const ResourceSpec& spec)
{
    const auto path = parameters.value(spec.key).toString();
    if (path.startsWith("asset:"))
        throw std::runtime_error(("方案资源尚未安装：" + spec.key).toStdString());
    return path.isEmpty()
        ? QDir(QCoreApplication::applicationDirPath() + "/../resource").filePath(spec.defaultFile)
        : path;
}
