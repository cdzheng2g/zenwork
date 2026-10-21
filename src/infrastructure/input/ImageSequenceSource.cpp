#include "infrastructure/input/ImageSequenceSource.h"

#include <opencv2/imgcodecs.hpp>

#include <QDir>
#include <QFileInfo>

namespace {
const QStringList kImageFilters { QStringLiteral("*.png"), QStringLiteral("*.jpg"), QStringLiteral("*.jpeg"),
    QStringLiteral("*.bmp"), QStringLiteral("*.tif"), QStringLiteral("*.tiff") };
}

bool ImageSequenceSource::setDirectory(const QString& directoryPath, QString* errorMessage)
{
    clear();

    const QDir directory(directoryPath);
    if (!directory.exists()) {
        if (errorMessage)
            *errorMessage = QStringLiteral("目录不存在: %1").arg(directoryPath);
        return false;
    }

    _directory = directory.absolutePath();
    _files = directory.entryList(kImageFilters, QDir::Files, QDir::Name | QDir::IgnoreCase);
    if (_files.isEmpty()) {
        if (errorMessage)
            *errorMessage = QStringLiteral("目录中没有支持的图片文件");
        _directory.clear();
        return false;
    }

    _index = 0;
    return true;
}

void ImageSequenceSource::clear()
{
    _directory.clear();
    _files.clear();
    _index = -1;
}

QString ImageSequenceSource::currentPath() const
{
    if (_index < 0 || _index >= _files.size())
        return { };
    return QDir(_directory).filePath(_files.at(_index));
}

QString ImageSequenceSource::currentFileName() const
{
    if (_index < 0 || _index >= _files.size())
        return { };
    return _files.at(_index);
}

bool ImageSequenceSource::hasNext() const
{
    return _index >= 0 && _index + 1 < _files.size();
}

bool ImageSequenceSource::hasPrevious() const
{
    return _index > 0 && _index < _files.size();
}

bool ImageSequenceSource::next()
{
    if (!hasNext())
        return false;
    ++_index;
    return true;
}

bool ImageSequenceSource::previous()
{
    if (!hasPrevious())
        return false;
    --_index;
    return true;
}

bool ImageSequenceSource::setIndex(int index)
{
    if (index < 0 || index >= _files.size())
        return false;
    _index = index;
    return true;
}

cv::Mat ImageSequenceSource::loadCurrent(QString* errorMessage) const
{
    const QString path = currentPath();
    if (path.isEmpty()) {
        if (errorMessage)
            *errorMessage = QStringLiteral("当前没有选择图片");
        return { };
    }

    cv::Mat image = cv::imread(path.toStdString(), cv::IMREAD_COLOR);
    if (image.empty() && errorMessage)
        *errorMessage = QStringLiteral("无法读取图片: %1").arg(path);
    return image;
}
