#ifndef REFERENCEIMAGEMANAGER_H
#define REFERENCEIMAGEMANAGER_H

#include "contracts/ReferenceImage.h"

#include <QMap>
#include <QString>

// 基准图管理:支持导入原始基准图、维护基准坐标(模板匹配的模板区域)、按产品型号
// 切换多组基准图。数据落地在 AppDataLocation/references/<modelName>/ 下:
//   image.png       原始基准图
//   meta.json       基准坐标(referenceRect)
// 这样每个产品型号一套独立目录,互不影响,后续新增型号只需import一次。
class ReferenceImageManager {
public:
    static ReferenceImageManager& instance();

    // 型号名会进入应用数据目录，必须是单个目录名，不能让导入写到目录之外。
    static bool isValidModelName(const QString& modelName);

    // 导入一张原始工业基准图,建立/覆盖名为modelName的基准图集
    bool importReferenceImage(const QString& modelName, const QString& sourceImagePath);
    // 从实时相机已获取的帧注册基准图；调用方可在信号回调结束后释放原始帧。
    bool importReferenceImage(const QString& modelName, const cv::Mat& liveFrame);

    // Worker-side persistence, followed by GUI-side publication of the completed snapshot.
    static bool savePreparedReference(ReferenceImage& reference, QString* error = nullptr);
    void adoptPreparedReference(const ReferenceImage& reference);

    // 切换当前使用的基准图集,返回是否存在该型号
    bool setCurrentModel(const QString& modelName);
    QString currentModel() const
    {
        return _currentModel;
    }

    bool setReferenceRect(const QString& modelName, const QRectF& rect);

    QStringList availableModels() const;
    bool contains(const QString& modelName) const;
    const ReferenceImage* referenceImage(const QString& modelName) const;
    const ReferenceImage* currentReferenceImage() const;

private:
    ReferenceImageManager();

    QString modelDir(const QString& modelName) const;
    QString imagePath(const QString& modelName) const;
    QString metaPath(const QString& modelName) const;

    void loadAllFromDisk();
    bool saveMeta(const QString& modelName) const;
    bool loadMeta(const QString& modelName, ReferenceImage& refImage) const;
    bool storeReferenceImage(const QString& normalizedName, const cv::Mat& image);

    QMap<QString, ReferenceImage> _references;
    QString _currentModel;
};

#endif // REFERENCEIMAGEMANAGER_H
