#ifndef RAMASSETGROUP_H
#define RAMASSETGROUP_H

#include "ramtemplateassetgroup.h"

#include "ramproject.h"

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

    DBTableModel *assets() const;
    int assetCount() const;
    RamProject *project() const;

    virtual QString details() const override;


public slots:
    virtual void edit(bool show = true) override;

protected:
    static QHash<QString, RamAssetGroup*> m_existingObjects;
    RamAssetGroup(QString uuid);
    virtual QString folderPath() const override;

    static QFrame *ui_assetGroupWidget;

private:
    void construct();

    DBTableModel *m_assets;
};

#endif // RAMASSETGROUP_H
