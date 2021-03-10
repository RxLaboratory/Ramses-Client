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

void RamSequence::update()
{
    _dbi->updateSequence(_uuid, _shortName, _name);
}
