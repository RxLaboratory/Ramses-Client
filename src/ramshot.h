#ifndef RAMSHOT_H
#define RAMSHOT_H

#include "ramobject.h"

class RamShot : public RamObject
{
    Q_OBJECT
public:
    explicit RamShot(QString shortName, QString name = "", QString sequenceUuid = "", QString uuid = "", QObject *parent = nullptr);
    ~RamShot();

    QString sequenceUuid() const;
    void setSequenceUuid(const QString &sequenceUuid);

    qreal duration() const;
    void setDuration(const qreal &duration);

    int order() const;
    void setOrder(int order);

    void update();

private:
    QString _sequenceUuid;
    qreal _duration = 0.0;
    int _order = -1;
    bool _orderChanged = false;
};

#endif // RAMSHOT_H
