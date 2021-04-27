#ifndef RAMSEQUENCE_H
#define RAMSEQUENCE_H

#include "ramshot.h"
#include "ramobjectlist.h"

class RamSequence : public RamObjectList
{
    Q_OBJECT
public:
    explicit RamSequence(QString shortName, QString name, QString projectUuid,  QString uuid = "", QObject *parent = nullptr);
    ~RamSequence();

    QString projectUuid() const;
    void setProjectUuid(const QString puuid);

    // Shots
    RamShot *shot(QString uuid) const;
    void append(RamShot *shot);
    void createShot(QString shortName = "NEW", QString name = "Shot");

    void update() Q_DECL_OVERRIDE;

private:
    QString _projectUuid;
};

#endif // RAMSEQUENCE_H
