#ifndef RAMASSETGROUP_H
#define RAMASSETGROUP_H

#include "ramobject.h"

class RamProject;
class RamObjectFilterModel;

class RamAssetGroup : public RamObject
{
    Q_OBJECT
public:

    // STATIC //

    static RamAssetGroup *getObject(QString uuid, bool constructNew = false);

    // OTHER //

    // Template (no project set)
    explicit RamAssetGroup(QString shortName, QString name);
    // Actual group
    explicit RamAssetGroup(QString shortName, QString name, RamProject *project);

    bool isTemplate() const;
    RamAssetGroup *createFromTemplate(RamProject *project);

    int assetCount() const;

    RamProject *project() const;

public slots:
    virtual void edit(bool show = true) override;

protected:
    RamAssetGroup(QString uuid);
    virtual QString folderPath() const override;

private:
    void construct();
    void setProject(RamProject *project);

    RamProject *m_project = nullptr;
    RamObjectFilterModel *m_assets;
};

#endif // RAMASSETGROUP_H
