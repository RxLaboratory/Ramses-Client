#ifndef RAMASSETGROUP_H
#define RAMASSETGROUP_H

#include "ramasset.h"
#include "ramobject.h"

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

    RamProject *project() const;

    static RamAssetGroup *assetGroup(QString uuid);

public slots:
    void update() override;
    virtual void edit() override;

private:
    bool _template;

    RamProject *m_project = nullptr;
};

#endif // RAMASSETGROUP_H
