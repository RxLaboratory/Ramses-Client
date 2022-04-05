#include "ramitem.h"

#include "ramproject.h"
#include "ramses.h"

RamItem::RamItem(QString shortName, RamProject *project, QString name, QString uuid) :
    RamObject(shortName, name, uuid, project)
{
    m_icon = ":/icons/asset";
    m_editRole = Admin;

    setObjectType(Item);
    m_productionType = RamStep::AssetProduction;
    m_project = project;

    this->setObjectName( "RamItem " + shortName);
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
        QList<QMetaObject::Connection> c;
        c << connect(stepHistory, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(insertStatus(QModelIndex,int,int)));
        c << connect(stepHistory, SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SLOT(removeStatus(QModelIndex,int,int)));
        c << connect(stepHistory, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)), this, SLOT(statusChanged(QModelIndex,QModelIndex)));
        c << connect(stepHistory, SIGNAL(rowsMoved(QModelIndex,int,int,QModelIndex,int)), this, SLOT(statusMoved(QModelIndex,int,int,QModelIndex,int)));
        c << connect(stepHistory, SIGNAL(modelReset()), this, SLOT(statusCleared()));
        m_statusConnections[ stepObj->uuid() ] = c;
        m_history[ step->uuid() ] = stepHistory;
    }
    return stepHistory;
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
}

void RamItem::insertStatus(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent)
    Q_UNUSED(first)

    RamStepStatusHistory *stepHistory = qobject_cast<RamStepStatusHistory*>( sender() );
    for (int i = first; i <= last; i++)
    {
        RamStatus *status = qobject_cast<RamStatus*>( stepHistory->at(i) );

        QString assigneduser;
        if( status->assignedUser() ) assigneduser = status->assignedUser()->uuid();

        if (this->objectType() == Asset) m_dbi->setAssetStatus(
                    m_uuid, status->state()->uuid(),
                    status->step()->uuid(),
                    status->user()->uuid(),
                    status->completionRatio(),
                    status->comment(),
                    status->version(),
                    status->uuid(),
                    assigneduser
                    );
        else if (this->objectType() == Shot) m_dbi->setShotStatus(
                    m_uuid,
                    status->state()->uuid(),
                    status->step()->uuid(),
                    status->user()->uuid(),
                    status->completionRatio(),
                    status->comment(),
                    status->version(),
                    status->uuid(),
                    assigneduser
                    );
    }

    if (last != stepHistory->count() - 1) return;

    stepHistory->step()->computeEstimation();

    emit statusChanged( this, stepHistory->step() );
}

void RamItem::statusChanged(const QModelIndex &first, const QModelIndex &last)
{
    Q_UNUSED(first)

    RamStepStatusHistory *stepHistory = qobject_cast<RamStepStatusHistory*>( sender() );
    int row = last.row();
    if (row != stepHistory->count() - 1) return;

    RamStep *step = stepHistory->step();
    if (!step) return;

    emit statusChanged( this, step );
}

void RamItem::remove(bool updateDB)
{
    QMapIterator<QString, RamStepStatusHistory* > i(m_history);
    i.toBack();
    while(i.hasPrevious())
    {
        i.previous();
        RamStepStatusHistory *r = i.value();
        r->deleteAll();
        r->deleteLater();
    }
    m_history.clear();
}

void RamItem::removeStatus(const QModelIndex &parent,int first ,int last)
{
    Q_UNUSED(parent)
    Q_UNUSED(first)

    RamStepStatusHistory *stepHistory = qobject_cast<RamStepStatusHistory*>( sender() );
    if (last != stepHistory->count() - 1) return;

    RamStep *step = stepHistory->step();
    if (!step) return;
    step->computeEstimation();
    emit statusChanged( this, step );
}

void RamItem::statusMoved(const QModelIndex &parent, int start, int end, const QModelIndex &destination, int row)
{
    Q_UNUSED(parent)
    Q_UNUSED(start)
    Q_UNUSED(destination)

    RamStepStatusHistory *stepHistory = qobject_cast<RamStepStatusHistory*>( sender() );
    if (end != stepHistory->count() - 1 && row != stepHistory->count() - 1) return;

    RamStep *step = stepHistory->step();
    if (!step) return;
    emit statusChanged( this, step );

}

void RamItem::statusCleared()
{
    RamStepStatusHistory *stepHistory = qobject_cast<RamStepStatusHistory*>( sender() );
    RamStep *step = stepHistory->step();
    if (!step) return;
    emit statusChanged( this, step );
}

RamProject *RamItem::project() const
{
    return m_project;
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
            if (nm.shortName().toLower() != m_shortName.toLower()) continue;
            return previewDir.filePath( file );
        }
    }

    // Not found, return the first one
    return previewDir.filePath( images.at(0) );
}
