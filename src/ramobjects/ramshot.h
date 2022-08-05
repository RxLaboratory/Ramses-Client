#ifndef RAMSHOT_H
#define RAMSHOT_H

#include "ramitem.h"

class RamSequence;
class RamAsset;
template<typename RO> class RamObjectList;

class RamShot : public RamItem
{
    Q_OBJECT
public:

    // STATIC METHODS //

    static RamShot *getObject(QString uuid, bool constructNew = false);

    // METHODS //

    RamShot(QString shortName, QString name, RamSequence *sequence);

    RamSequence *sequence() const;
    void setSequence(RamSequence *sequence);

    qreal duration() const;
    void setDuration(const qreal &duration);

    RamObjectList<RamAsset *> *assets() const;

    virtual QString filterUuid() const override;

    virtual QString details() const override;

public slots:
    virtual void edit(bool show = true) override;

protected:
    RamShot(QString uuid);
    virtual QString folderPath() const override;

private:
    void construct();

    RamObjectList<RamAsset*> *m_assets;
};

#endif // RAMSHOT_H
