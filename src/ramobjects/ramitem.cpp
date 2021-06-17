#include "ramitem.h"

#include "ramproject.h"

RamItem::RamItem(QString shortName, RamProject *project, QString name, QString uuid, QObject *parent) :
    RamObject(shortName, name, uuid, parent)
{
    setObjectType(Item);
    m_productionType = RamStep::AssetProduction;
    _statusHistory = new RamObjectUberList("Status history", this);
    m_project = project;

    this->setObjectName( "RamItem " + shortName);
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

    for (int i = 0; i < _statusHistory->count(); i++)
    {
        RamStepStatusHistory *history = qobject_cast<RamStepStatusHistory*>( _statusHistory->at(i) );
        if (history->step()->is(step))
        {
            history->append( status );
            break;
        }
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

void RamItem::newStep(RamObject *obj)
{
    RamStep *step = qobject_cast<RamStep*>( obj );

    if (m_productionType != step->type() ) return;

    RamStepStatusHistory *history = new RamStepStatusHistory(step, this);
    _statusHistory->append(history);
}

void RamItem::stepRemoved(RamObject *step)
{
    for (int i = 0; i < _statusHistory->count(); i++)
    {
        RamStepStatusHistory *history = qobject_cast<RamStepStatusHistory*>( _statusHistory->at(i) );
        if (history->step()->is(step))
        {
            history->remove();
            break;
        }
    }
}

RamStep::Type RamItem::productionType() const
{
    return m_productionType;
}

RamItem *RamItem::item(QString uuid)
{
    return qobject_cast<RamItem*>( RamObject::obj(uuid) );
}

void RamItem::setProductionType(RamStep::Type newProductionType)
{
    m_productionType = newProductionType;

    for (int i = 0; i < m_project->steps()->count(); i++)
        newStep( m_project->steps()->at(i));

    connect(m_project->steps(), &RamObjectList::objectAdded, this, &RamItem::newStep);
    connect(m_project->steps(), &RamObjectList::objectRemoved, this, &RamItem::stepRemoved);
}

RamProject *RamItem::project() const
{
    return m_project;
}
