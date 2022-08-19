#include "ramabstractitem.h"

#include "ramses.h"

// PUBLIC //

RamAbstractItem::RamAbstractItem(QString shortName, QString name, ObjectType type, RamProject *project) :
    RamObject(shortName, name, type, project)
{
    construct();

    setProject(project);
}

RamAbstractItem::RamAbstractItem(QString uuid, ObjectType type):
    RamObject(uuid, type)
{
    construct();

    QJsonObject d = data();

    // Get (and clean) the status history
    QJsonObject history = d.value("statusHistory").toObject();
    QStringList stepUuids = history.keys();
    for (int i = 0; i < stepUuids.count(); i++)
    {
        QString stepUuid = stepUuids[i];
        QString historyUuid = history.value(stepUuid).toString();
        RamStepStatusHistory *stepHistory = RamStepStatusHistory::get(historyUuid);
        if (stepHistory)
        {
            if ((stepHistory->step()->type() != RamStep::ShotProduction && type == Shot) ||
                (stepHistory->step()->type() != RamStep::AssetProduction && type == Asset) )
            {
                stepHistory->remove();
                stepHistory->deleteLater();
                continue;
            }

            m_history[ stepUuid ] = stepHistory;
            connectHistory(stepHistory);
        }
    }
    saveHistory();
}

RamProject *RamAbstractItem::project() const
{
    return RamProject::get( getData("project").toString("none") );
}

QMap<QString, RamStepStatusHistory*> RamAbstractItem::statusHistory() const
{
    return m_history;
}

RamStepStatusHistory *RamAbstractItem::statusHistory(RamObject *stepObj)
{
    if (!stepObj) return nullptr;

    RamStepStatusHistory *stepHistory = m_history.value( stepObj->uuid(), nullptr );
    if (!stepHistory)
    {
        RamStep *step = RamStep::c( stepObj );
        stepHistory = new RamStepStatusHistory(step, this);
        m_history[step->uuid()] = stepHistory;
        saveHistory();
        connectHistory(stepHistory);
    }
    return stepHistory;
}

RamStatus *RamAbstractItem::setStatus(RamUser *user, RamState *state, RamStep *step, int completionRatio, QString comment, int version)
{
    RamStatus *newStatus = new RamStatus(user, this, step );
    newStatus->setState(state);
    if (completionRatio >= 0) newStatus->setCompletionRatio(completionRatio);
    if (comment != "") newStatus->setComment(comment);
    newStatus->setVersion(version);

    addStatus(newStatus);

    return newStatus;
}

void RamAbstractItem::addStatus(RamStatus *status)
{
    RamStep *step = status->step();
    if (!step) return;

    // Check if there's a user assigned
    if (!status->assignedUser())
        status->assignUser( assignedUser(step) );

    RamStepStatusHistory *history = statusHistory(step);
    history->append( status );
    history->sort();
}

RamStatus *RamAbstractItem::status(RamStep *step)
{
    // Get the new current status
    RamStepStatusHistory *history = statusHistory(step);

    if (history->rowCount() == 0) return nullptr;

    history->sort();
    RamStatus *currentStatus = qobject_cast<RamStatus*> ( history->last() );
    if (!currentStatus) return nullptr;

    return currentStatus;
}

QList<RamStatus *> RamAbstractItem::status()
{
    QList<RamStatus *> statuses;
    if (m_history.count() == 0) return statuses;
    QMapIterator<QString, RamStepStatusHistory*> i(m_history);
    while(i.hasNext())
    {
        i.next();
        RamStepStatusHistory *h = i.value();
        h->sort();
        statuses << qobject_cast<RamStatus*>( h->last() );
    }
    return statuses;
}

RamUser *RamAbstractItem::assignedUser(RamStep *step)
{
    RamStatus *previous = status(step);
    if (previous)
        return previous->assignedUser();

    return nullptr;
}

bool RamAbstractItem::isUserAssigned(RamObject *u, RamStep *step)
{
    if(step)
    {
        RamStatus *s = status(step);
        if (!s) return false;
        if ( !s->assignedUser() ) return false;
        return s->assignedUser()->is(u);
    }

    QList<RamStatus*> s = status();
    for (int i = 0; i < s.count(); i++)
    {
        if (!s.at(i)) continue;
        if (!s.at(i)->assignedUser()) continue;
        if (s.at(i)->assignedUser()->is(u)) return true;
    }
    return false;
}

bool RamAbstractItem::isUnassigned(RamStep *step)
{
    if (step)
    {
        RamStatus *s = status(step);
        if (!s) return true;
        if ( !s->assignedUser() ) return true;
        return false;
    }

    QList<RamStatus*> s = status();
    for (int i = 0; i < s.count(); i++)
    {
        if (!s.at(i)) return true;
        if (!s.at(i)->assignedUser()) return true;
    }
    return false;
}

bool RamAbstractItem::hasState(RamObject *state, RamStep *step)
{
    RamState *noState = Ramses::instance()->noState();

    if(step)
    {
        RamStatus *s = status(step);
        if (!s && noState->is(state)) return true;
        if (!s) return false;
        if( !s->state() && noState->is(state)) return true;
        if( !s->state()) return false;
        return s->state()->is(state);
    }

    QList<RamStatus*> s = status();

    for (int i = 0; i < s.count(); i++)
    {
        if(!s.at(i) && state->is(noState)) return true;
        if(!s.at(i)) continue;
        if(!s.at(i)->state() && state->is(noState)) return true;
        if(!s.at(i)->state()) continue;
        if(s.at(i)->state()->is(state)) return true;
    }
    return false;
}

// PROTECTED //

void RamAbstractItem::historyChanged()
{
    RamStepStatusHistory *h = RamStepStatusHistory::c( QObject::sender() );
    emit statusChanged(this, h->step());
    emit dataChanged(this);
}

// PRIVATE //

void RamAbstractItem::construct()
{
    m_icon = ":/icons/asset";
    m_editRole = Admin;
}

void RamAbstractItem::setProject(RamProject *proj)
{
    insertData("project", proj->uuid());
    setParent(proj);
}

void RamAbstractItem::saveHistory()
{
    QJsonObject history;
    QMapIterator<QString, RamStepStatusHistory*> i(m_history);
    while (i.hasNext())
    {
        i.next();
        history.insert(i.key(), i.value()->uuid());
    }
    insertData("statusHistory", history);
}

void RamAbstractItem::connectHistory(RamStepStatusHistory *history)
{
    connect(history, &RamStepStatusHistory::dataChanged, this, &RamAbstractItem::historyChanged);
    connect(history, &RamStepStatusHistory::listChanged, this, &RamAbstractItem::historyChanged);
}

