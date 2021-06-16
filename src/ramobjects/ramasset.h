#ifndef RAMASSET_H
#define RAMASSET_H

#include "ramitem.h"

class RamProject;
class RamAssetGroup;

class RamAsset : public RamItem
{
    Q_OBJECT
public:
    explicit RamAsset(QString shortName, RamProject *project, RamAssetGroup *assetGroup, QString name = "",  QString uuid = "", QObject *parent = nullptr);
    ~RamAsset();

    RamAssetGroup *assetGroup() const;
    void setAssetGroup(RamAssetGroup *assetGroup);

    QStringList tags() const;
    void setTags(QString tags);
    void addTag(QString tag);
    void removeTag(QString tag);
    bool hasTag(QString tag);

    void update();

    static RamAsset *asset(QString uuid);

private:
    QStringList _tags;
    // Containers
    RamAssetGroup *m_assetGroup;
};

#endif // RAMASSET_H
