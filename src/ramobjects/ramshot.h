#ifndef RAMSHOT_H
#define RAMSHOT_H

#include "ramitem.h"

class RamSequence;

class RamShot : public RamItem
{
    Q_OBJECT
public:
    explicit RamShot(QString shortName, RamSequence *sequence, QString name = "", QString uuid = "");
    ~RamShot();

    RamSequence *sequence() const;
    void setSequence(RamSequence *sequence);

    qreal duration() const;
    void setDuration(const qreal &duration);

    RamObjectList *assets() const;

    static RamShot *shot(QString uuid);

public slots:
    void update() override;
    virtual void edit(bool show = true) override;

protected:
    virtual QString folderPath() const override;

private slots:
    void assetAssigned(const QModelIndex &parent, int first, int last);
    void assetUnassigned(const QModelIndex &parent, int first, int last);
    virtual void removeFromDB() override;

private:
    RamSequence *m_sequence;
    qreal m_duration = 0.0;
    RamObjectList *m_assets;

    QMetaObject::Connection m_sequenceConnection;
};

#endif // RAMSHOT_H
