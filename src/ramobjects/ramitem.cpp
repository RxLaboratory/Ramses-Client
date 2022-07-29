#include "ramitem.h"

#include "ramproject.h"
#include "ramses.h"

// STATIC //

RamItem *RamItem::getObject(QString uuid, bool constructNew)
{
    RamObject *obj = RamObject::getObject(uuid);
    if (!obj && constructNew) return new RamItem( uuid );
    return qobject_cast<RamItem*>( obj );
}

// PUBLIC //

RamItem::RamItem(QString shortName, QString name, RamStep::Type productionType, RamProject *project) :
    RamObject(shortName, name, Item, project)
{
    construct();

    m_project = project;
    m_productionType = productionType;
}

RamProject *RamItem::project() const
{
    return m_project;
}

RamStep::Type RamItem::productionType() const
{
    return m_productionType;
}

QMap<QString, RamStepStatusHistory*> RamItem::statusHistory() const
{
    return m_history;
}

RamStepStatusHistory *RamItem::statusHistory(RamObject *stepObj)
{
    if (!stepObj) return nullptr;

    RamStepStatusHistory *stepHistory = m_history.value( stepObj->uuid(), nullptr );
    if (!stepHistory)
    {
        RamStep *step = qobject_cast<RamStep*>( stepObj );
        stepHistory = new RamStepStatusHistory(step, this);
        m_history[ step->uuid() ] = stepHistory;
    }
    return stepHistory;
}

RamStatus *RamItem::setStatus(RamUser *user, RamState *state, RamStep *step, int completionRatio, QString comment, int version)
{
    RamStatus *newStatus = new RamStatus(user, state, step, this);
    if (completionRatio >= 0) newStatus->setCompletionRatio(completionRatio);
    if (comment != "") newStatus->setComment(comment);
    newStatus->setVersion(version);


    addStatus(newStatus);

    return newStatus;
}

void RamItem::addStatus(RamStatus *status)
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

RamStatus *RamItem::status(RamObject *step)
{
    // Get the new current status
    RamStepStatusHistory *history = statusHistory(step);

    if (history->count() == 0) return nullptr;

    history->sort();
    RamStatus *currentStatus = qobject_cast<RamStatus*> ( history->last() );
    if (!currentStatus) return nullptr;

    return currentStatus;
}

QList<RamStatus *> RamItem::status()
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

RamUser *RamItem::assignedUser(RamStep *step)
{
    RamStatus *previous = status(step);
    if (previous)
        return previous->assignedUser();

    return nullptr;
}

bool RamItem::isUserAssigned(RamUser *u, RamStep *step)
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

bool RamItem::isUnassigned(RamStep *step)
{
    if(step)
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

bool RamItem::hasState(RamState *state, RamStep *step)
{
    RamState *noState = Ramses::instance()->noState();

    if(step)
    {
        RamStatus *s = status(step);
        if (!s && state->is(noState)) return true;
        if (!s) return false;
        if( !s->state() && state->is(noState)) return true;
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

QString RamItem::previewImagePath() const
{
    QDir previewDir = path(RamObject::PreviewFolder);
    QStringList filters;
    filters << "*.jpg" << "*.png" << "*.jpeg" << "*.gif";
    QStringList images = previewDir.entryList(filters, QDir::Files );

    if (images.count() == 0) return "";

    RamNameManager nm;

    foreach(QString file, images)
    {
        if (nm.setFileName(file))
        {
            if (nm.project().toLower() != m_project->shortName().toLower()) continue;
            if (nm.shortName().toLower() != shortName().toLower()) continue;
            return previewDir.filePath( file );
        }
    }

    // Not found, return the first one
    return previewDir.filePath( images.at(0) );
}

// PROTECTED //

RamItem::RamItem(QString uuid):
    RamObject(uuid, ObjectType::Item)
{
    construct();

    // Set the project this item belongs to
    QJsonObject d = data();
    m_project = RamProject::getObject(d.value("project").toString(), true);
    this->setParent( m_project );

    // Set the type
    m_productionType = RamStep::stepTypeFromName( d.value("productionType").toString("AssetProduction") );

    // Get the status history
    QJsonObject history = d.value("statusHistory").toObject();
    QStringList stepUuids = history.keys();
    for (int i = 0; i < stepUuids.count(); i++)
    {
        QString stepUuid = stepUuids[i];
        QString historyUuid = history.value(stepUuid).toString();
        RamStepStatusHistory *stepHistory = RamStepStatusHistory::getObject(historyUuid, true);
        m_history[ stepUuid ] = stepHistory;
    }
}

// PRIVATE //

void RamItem::construct()
{
    m_icon = ":/icons/asset";
    m_editRole = Admin;
}
