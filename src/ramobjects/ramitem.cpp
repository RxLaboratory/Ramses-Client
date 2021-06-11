#include "ramitem.h"

RamItem::RamItem(QString shortName, QString name, QString uuid, QObject *parent) :
    RamObject(shortName, name, uuid, parent)
{
    _statusHistory = new RamObjectUberList(this);

    this->setObjectName( "RamItem" );
}

void RamItem::setStatus(RamUser *user, RamState *state, RamStep *step, int completionRatio, QString comment, int version)
{
    RamStatus *status = new RamStatus(user, state, step, this);
    if (completionRatio >= 0) status->setCompletionRatio(completionRatio);
    if (comment != "") status->setComment(comment);
    status->setVersion(version);

    if (this->objectType() == Asset) _dbi->setAssetStatus(_uuid, state->uuid(), step->uuid(), user->uuid(), completionRatio, comment, version, status->uuid());
    else if (this->objectType() == Shot) _dbi->setShotStatus(_uuid, state->uuid(), step->uuid(), user->uuid(), completionRatio, comment, version, status->uuid());

    addStatus(status);
}

void RamItem::addStatus(RamStatus *status)
{
    RamStep *step = status->step();
    if (!step) return;
    RamObject *o = _statusHistory->fromUuid(step->uuid());
    if (o)
    {
        RamStepStatusHistory *history = qobject_cast<RamStepStatusHistory*>( o );
        history->append( status );
    }
    else
    {
        RamStepStatusHistory *history = new RamStepStatusHistory(step, this);
        history->append( status );
        _statusHistory->append( history );
    }
}

RamObjectUberList *RamItem::statusHistory() const
{
    return _statusHistory;
}

RamStepStatusHistory *RamItem::statusHistory(RamStep *step) const
{
    return statusHistory( step->uuid() );
}

RamStepStatusHistory *RamItem::statusHistory(QString stepUuid) const
{
    RamStepStatusHistory *history = qobject_cast<RamStepStatusHistory*>( _statusHistory->fromUuid( stepUuid ) );

    return history;
}

RamStatus *RamItem::status(QString stepUuid) const
{
    RamStepStatusHistory *history = qobject_cast<RamStepStatusHistory*>( _statusHistory->fromUuid( stepUuid ) );
    if (history)
    {
        history->sort();
        return qobject_cast<RamStatus*> ( history->last() );
    }
    return nullptr;
}
