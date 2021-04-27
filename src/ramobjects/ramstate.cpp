#include "ramstate.h"

RamState::RamState(QString shortName, QString name, QString uuid, QObject *parent) :
    RamObject(shortName, name, uuid, parent)
{
    this->setObjectType(State);
    _dbi->createState(_shortName, _name, _uuid);
}

RamState::~RamState()
{
    _dbi->removeState(_uuid);
}

QColor RamState::color() const
{
    return _color;
}

void RamState::setColor(const QColor &color)
{
    _color = color;
    emit changed(this);
}

int RamState::completionRatio() const
{
    return _completionRatio;
}

void RamState::setCompletionRatio(int completionRatio)
{
    _completionRatio = completionRatio;
    emit changed(this);
}

void RamState::update()
{
    _dbi->updateState(_uuid, _shortName, _name, _color.name(), QString::number(_completionRatio));
}
