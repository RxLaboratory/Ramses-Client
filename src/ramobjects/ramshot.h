#ifndef RAMSHOT_H
#define RAMSHOT_H

#include "ramitem.h"

class RamProject;
class RamSequence;

class RamShot : public RamItem
{
    Q_OBJECT
public:
    explicit RamShot(QString shortName, RamProject *project, RamSequence *sequence, QString name = "", QString uuid = "");
    ~RamShot();

    RamSequence *sequence() const;
    void setSequence(RamSequence *sequence);

    qreal duration() const;
    void setDuration(const qreal &duration);

    static RamShot *shot(QString uuid);

public slots:
    void update();

private:
    RamSequence *m_sequence;
    qreal m_duration = 0.0;
};

#endif // RAMSHOT_H
