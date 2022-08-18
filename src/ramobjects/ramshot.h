#ifndef RAMSHOT_H
#define RAMSHOT_H

#include "ramitem.h"

class RamSequence;
class RamAsset;
class RamObjectList;

class RamShot : public RamItem
{
    Q_OBJECT
public:

    // STATIC METHODS //

    static RamShot *get(QString uuid);
    // Short for qobject_cast<RamShot*>
    static RamShot *c(RamObject *obj);

    // METHODS //

    RamShot(QString shortName, QString name, RamSequence *sequence);
    RamShot(QString uuid);

    RamSequence *sequence() const;
    void setSequence(RamObject *sequence);

    qreal duration() const;
    void setDuration(const qreal &duration);

    RamObjectList *assets() const;
    RamAsset *assetAt(int row) const;

    virtual QString filterUuid() const override;

    virtual QString details() const override;

public slots:
    virtual void edit(bool show = true) override;

protected:
    virtual QString folderPath() const override;

private:
    void construct();
    void getCreateLists();

    RamObjectList *m_assets;
};

#endif // RAMSHOT_H
