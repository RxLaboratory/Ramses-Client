#ifndef RAMASSET_H
#define RAMASSET_H

#include "ramobject.h"

class RamAsset : public RamObject
{
    Q_OBJECT
public:
    explicit RamAsset(QString shortName, QString name = "", QString assetGroupUuid = "",  QString uuid = "", QObject *parent = nullptr);
    ~RamAsset();

    QString assetGroupUuid() const;
    void setAssetGroupUuid(const QString &assetGroupUuid);

    QStringList tags() const;
    void setTags(QString tags);
    void addTag(QString tag);
    void removeTag(QString tag);
    bool hasTag(QString tag);

    void update();

private:
    QStringList _tags;
    QString _assetGroupUuid;
};

#endif // RAMASSET_H
