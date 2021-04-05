#ifndef RAMSEQUENCE_H
#define RAMSEQUENCE_H

#include "ramshot.h"

class RamSequence : public RamObject
{
    Q_OBJECT
public:
    explicit RamSequence(QString shortName, QString name, QString projectUuid,  QString uuid = "", QObject *parent = nullptr);
    ~RamSequence();

    QString projectUuid() const;
    void setProjectUuid(const QString puuid);

    // Shots
    QList<RamShot *> shots() const;
    RamShot *shot(QString uuid) const;
    void addShot(RamShot *shot);
    void createShot(QString shortName = "NEW", QString name = "Shot");
    void removeShot(QString uuid);
    void removeShot(RamObject *shot);
    void sortShots();

    void update();

public slots:
    void shotRemoved(RamObject *o);

signals:
    void newShot(RamShot *);
    void shotRemoved(QString uuid);

private:
    QString _projectUuid;
    QList<RamShot*> _shots;
};

#endif // RAMSEQUENCE_H
