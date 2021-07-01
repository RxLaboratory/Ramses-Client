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

    static RamAsset *asset(QString uuid);

public slots:
    void update() override;
    virtual void edit(bool show = true) override;
    virtual void removeFromDB() override;

protected:
    virtual QString folderPath() const override;

private:
    QStringList _tags;
    // Containers
    RamAssetGroup *m_assetGroup;

    QMetaObject::Connection m_assetGroupConnection;
};

#endif // RAMASSET_H
