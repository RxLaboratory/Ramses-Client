#ifndef RAMTEMPLATEASSETGROUP_H
#define RAMTEMPLATEASSETGROUP_H

#include "ramobject.h"

class RamTemplateAssetGroup : public RamObject
{
    Q_OBJECT
public:

    //STATIC //

    static RamTemplateAssetGroup *getObject(QString uuid, bool constructNew = false);
    static RamTemplateAssetGroup *c(RamObject *o);

    // OTHER //

    explicit RamTemplateAssetGroup(QString shortName, QString name);

public slots:
    virtual void edit(bool show = true) override;

protected:
    RamTemplateAssetGroup(QString uuid);

private:
    void construct();
};

#endif // RAMTEMPLATEASSETGROUP_H
