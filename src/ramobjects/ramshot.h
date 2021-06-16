#ifndef RAMSHOT_H
#define RAMSHOT_H

#include "ramitem.h"

class RamProject;

class RamShot : public RamItem
{
    Q_OBJECT
public:
    explicit RamShot(QString shortName, RamProject *project, QString name = "", QString sequenceUuid = "", QString uuid = "", QObject *parent = nullptr);
    ~RamShot();

    QString sequenceUuid() const;
    void setSequenceUuid(const QString &sequenceUuid);

    qreal duration() const;
    void setDuration(const qreal &duration);

    static RamShot *shot(QString uuid);

public slots:
    void update();

private:
    QString _sequenceUuid;
    qreal _duration = 0.0;
};

#endif // RAMSHOT_H
