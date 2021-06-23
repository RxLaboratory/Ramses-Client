#ifndef RAMASSET_H
#define RAMASSET_H

#include "ramitem.h"

class RamProject;
class RamAssetGroup;

class RamAsset : public RamItem
{
    Q_OBJECT
public:
    explicit RamAsset(QString shortName, RamAssetGroup *assetGroup, QString name = "",  QString uuid = "");
    ~RamAsset();

    RamAssetGroup *assetGroup() const;
    void setAssetGroup(RamAssetGroup *assetGroup);

    QStringList tags() const;
    void setTags(QString tags);
    void addTag(QString tag);
    void removeTag(QString tag);
    bool hasTag(QString tag);

    void update() override;

    static RamAsset *asset(QString uuid);

public slots:
    virtual void edit(bool show = true) override;

private:
    QStringList _tags;
    // Containers
    RamAssetGroup *m_assetGroup;

    QMetaObject::Connection m_assetGroupConnection;
};

#endif // RAMASSET_H
