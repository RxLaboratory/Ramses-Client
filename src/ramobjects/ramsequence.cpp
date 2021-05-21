#include "ramsequence.h"

RamSequence::RamSequence(QString shortName, QString name, QString projectUuid, QString uuid, QObject *parent):
    RamObjectList(shortName, name, uuid, parent)
{
    this->setObjectType(Sequence);
    _projectUuid = projectUuid;
    _dbi->createSequence(_shortName, _name, projectUuid, _uuid);
}

RamSequence::~RamSequence()
{
    _dbi->removeSequence(_uuid);
}

QString RamSequence::projectUuid() const
{
    return _projectUuid;
}

void RamSequence::setProjectUuid(const QString puuid)
{
    _projectUuid = puuid;
}

RamShot *RamSequence::shot(QString uuid) const
{
    return qobject_cast<RamShot*>( this->fromUuid(uuid) );
}

void RamSequence::append(RamShot *shot)
{
    shot->setSequenceUuid( _uuid );
    shot->update();
    RamObjectList::append(shot);
}

void RamSequence::createShot(QString shortName, QString name)
{
    RamShot *shot = new RamShot(shortName, name, _uuid);
    append(shot);
}

void RamSequence::update()
{
    _dbi->updateSequence(_uuid, _shortName, _name);
}
