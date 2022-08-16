#ifndef RAMASSETGROUP_H
#define RAMASSETGROUP_H

#include "ramtemplateassetgroup.h"

#include "ramproject.h"
#include "data-models/ramobjectfiltermodel.h"
class RamAsset;

class RamAssetGroup : public RamTemplateAssetGroup
{
    Q_OBJECT
public:

    // STATIC //

    static RamAssetGroup *get(QString uuid);
    static RamAssetGroup *c(RamObject *o);
    static RamAssetGroup *createFromTemplate(RamTemplateAssetGroup *tempAG, RamProject *project);

    // OTHER //

    explicit RamAssetGroup(QString shortName, QString name, RamProject *project);
    RamAssetGroup(QString uuid);

    int assetCount() const;
    RamProject *project() const;

    virtual QString details() const override;

public slots:
    virtual void edit(bool show = true) override;

protected:
    virtual QString folderPath() const override;

private:
    void construct();
    void setProject(RamProject *project);

    RamProject *m_project = nullptr;
    RamObjectFilterModel *m_assets;
};

#endif // RAMASSETGROUP_H
