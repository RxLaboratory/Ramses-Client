#ifndef RAMTEMPLATEASSETGROUP_H
#define RAMTEMPLATEASSETGROUP_H

#include "ramobject.h"

class RamTemplateAssetGroup : public RamObject
{
    Q_OBJECT
public:

    //STATIC //

    static RamTemplateAssetGroup *get(QString uuid );
    static RamTemplateAssetGroup *c(RamObject *o);

    // OTHER //

    explicit RamTemplateAssetGroup(QString shortName, QString name);
    RamTemplateAssetGroup(QString uuid);

public slots:
    virtual void edit(bool show = true) override;

private:
    void construct();
};

#endif // RAMTEMPLATEASSETGROUP_H
