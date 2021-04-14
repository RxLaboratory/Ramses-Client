#include "ramsequence.h"

RamSequence::RamSequence(QString shortName, QString name, QString projectUuid, QString uuid, QObject *parent):
    RamObject(shortName, name, uuid, parent)
{
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

QList<RamShot *> RamSequence::shots() const
{
    return _shots;
}

RamShot *RamSequence::shot(QString uuid) const
{
    for (RamShot *s: _shots)
    {
        if (s->uuid() == uuid) return s;
    }
    return nullptr;
}

void RamSequence::addShot(RamShot *shot)
{
    _shots << shot;
    shot->setSequenceUuid( _uuid );
    shot->update();
    connect(shot, SIGNAL(removed(RamObject*)), this, SLOT(shotRemoved(RamObject*)));
    emit newShot(shot);
}

void RamSequence::createShot(QString shortName, QString name)
{
    RamShot *shot = new RamShot(shortName, name, _uuid);
    addShot(shot);
}

void RamSequence::removeShot(QString uuid)
{
    for (int i = _shots.count() -1; i >= 0; i--)
    {
        RamShot *s = _shots[i];
        if (s->uuid() == uuid)
        {
            RamShot *shot = _shots.takeAt(i);
            emit shotRemovedFromSequence(shot);
        }
    }
}

void RamSequence::removeShot(RamObject *shot)
{
    removeShot(shot->uuid());
}

bool shotSorter(RamShot *a, RamShot *b)
{
    return a->order() > b->order();
}

void RamSequence::sortShots()
{
    std::sort(_shots.begin(), _shots.end(), shotSorter);
}

void RamSequence::update()
{
    _dbi->updateSequence(_uuid, _shortName, _name);
}

void RamSequence::shotRemoved(RamObject *o)
{
    removeShot(o);
}
