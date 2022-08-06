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

    static RamAssetGroup *getObject(QString uuid, bool constructNew = false);
    static RamAssetGroup *createFromTemplate(RamTemplateAssetGroup *tempAG, RamProject *project);

    // OTHER //

    // Actual group
    explicit RamAssetGroup(QString shortName, QString name, RamProject *project);

    int assetCount() const;
    RamProject *project() const;

    virtual QString details() const override;

public slots:
    virtual void edit(bool show = true) override;

protected:
    RamAssetGroup(QString uuid);
    virtual QString folderPath() const override;

private:
    void construct();
    void setProject(RamProject *project);

    RamProject *m_project = nullptr;
    RamObjectFilterModel<RamAsset*> *m_assets;
};

#endif // RAMASSETGROUP_H
