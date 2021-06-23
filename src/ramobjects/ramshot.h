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

    static RamShot *shot(QString uuid);

public slots:
    void update() override;

public slots:
    virtual void edit(bool show = true) override;

private:
    RamSequence *m_sequence;
    qreal m_duration = 0.0;

    QMetaObject::Connection m_sequenceConnection;
};

#endif // RAMSHOT_H
