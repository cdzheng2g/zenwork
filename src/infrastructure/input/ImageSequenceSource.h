#ifndef IMAGESEQUENCESOURCE_H
#define IMAGESEQUENCESOURCE_H

#include <opencv2/core.hpp>

#include <QString>
#include <QStringList>

// 文件夹图片输入源:按文件名稳定排序管理一组测试图像,供运行管理工作台
// 在没有相机或需要回放固定样本时使用。只记录路径,当前帧由loadCurrent()读取。
class ImageSequenceSource {
public:
    bool setDirectory(const QString& directoryPath, QString* errorMessage = nullptr);
    void clear();

    bool isEmpty() const
    {
        return _files.isEmpty();
    }
    int count() const
    {
        return _files.size();
    }
    int index() const
    {
        return _index;
    }
    QString directory() const
    {
        return _directory;
    }
    QString currentPath() const;
    QString currentFileName() const;

    bool hasNext() const;
    bool hasPrevious() const;
    bool next();
    bool previous();
    bool setIndex(int index);

    cv::Mat loadCurrent(QString* errorMessage = nullptr) const;

private:
    QString _directory;
    QStringList _files;
    int _index { -1 };
};

#endif // IMAGESEQUENCESOURCE_H
