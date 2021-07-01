#ifndef RAMASSETGROUP_H
#define RAMASSETGROUP_H

#include "ramasset.h"
#include "ramobject.h"
#include "data-models/ramobjectfiltermodel.h"

class RamProject;

class RamAssetGroup : public RamObject
{
    Q_OBJECT
public:
    // Template (no project set)
    explicit RamAssetGroup(QString shortName, QString name = "",  QString uuid = "" );
    // Actual group
    explicit RamAssetGroup(QString shortName, RamProject *project, QString name,  QString uuid = "");
    ~RamAssetGroup();

    bool isTemplate() const;
    RamAssetGroup *createFromTemplate(RamProject *project);

    int assetCount() const;

    RamProject *project() const;

    static RamAssetGroup *assetGroup(QString uuid);

public slots:
    void update() override;
    virtual void edit(bool show = true) override;
    virtual void removeFromDB() override;

protected:
    virtual QString folderPath() const override;

private:
    bool m_template;

    RamProject *m_project = nullptr;
    RamObjectFilterModel *m_assets;
};

#endif // RAMASSETGROUP_H
