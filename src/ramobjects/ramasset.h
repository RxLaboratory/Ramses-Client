#ifndef RAMASSET_H
#define RAMASSET_H

#include "ramitem.h"

class RamProject;
class RamAssetGroup;

class RamAsset : public RamItem
{
    Q_OBJECT
public:

    // STATIC //

    static RamAsset *getObject(QString uuid, bool constructNew = false);

    // OTHER //

    RamAsset(QString shortName, QString name, RamAssetGroup *ag, RamProject *project);

    RamAssetGroup *assetGroup() const;
    void setAssetGroup(RamAssetGroup *assetGroup);

    QStringList tags() const;
    void setTags(QString tags);
    void addTag(QString tag);
    void removeTag(QString tag);
    bool hasTag(QString tag);

    QString filterUuid() const override;

public slots:
    virtual void edit(bool show = true) override;

protected:
    RamAsset(QString uuid);
    virtual QString folderPath() const override;

private:
    void construct();
};

#endif // RAMASSET_H
