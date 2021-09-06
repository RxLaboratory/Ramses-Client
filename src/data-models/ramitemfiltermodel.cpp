#include "ramitemfiltermodel.h"

RamItemFilterModel::RamItemFilterModel(QObject *parent) : RamObjectFilterModel(parent)
{

}

void RamItemFilterModel::useFilters(bool use)
{
    m_userFilters = use;
}

void RamItemFilterModel::hideUser(RamUser *u)
{
    m_users.removeAll(u);
    invalidateFilter();
}

void RamItemFilterModel::showUser(RamUser *u)
{
    if (!m_users.contains(u)) m_users << u;
    invalidateFilter();
}

void RamItemFilterModel::clearUsers()
{
    m_users.clear();
    invalidateFilter();
}

void RamItemFilterModel::showUnassigned(bool show)
{
    m_showUnassigned = show;
    invalidateFilter();
}

void RamItemFilterModel::hideState(RamState *s)
{
    m_states.removeAll(s);
    invalidateFilter();
}

void RamItemFilterModel::showState(RamState *s)
{
    if (!m_states.contains(s))
    {
        m_states << s;
        invalidateFilter();
    }
}

void RamItemFilterModel::clearStates()
{
    m_states.clear();
    invalidateFilter();
}

void RamItemFilterModel::setStepType(RamStep::Type t)
{
    m_stepType = t;
    invalidateFilter();
}

void RamItemFilterModel::hideStep(RamStep *s)
{
    if (!m_hiddenSteps.contains(s))
    {
        m_hiddenSteps << s;
        invalidateFilter();
    }
}

void RamItemFilterModel::showStep(RamStep *s)
{
    m_hiddenSteps.removeAll(s);
    invalidateFilter();
}

void RamItemFilterModel::showAllSteps()
{
    m_hiddenSteps.clear();
    invalidateFilter();
}

bool RamItemFilterModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    // If not accepted by the search/uuid filter, return
    if(! RamObjectFilterModel::filterAcceptsRow(sourceRow, sourceParent)) return false;

    if (!m_userFilters) return true;

    QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);
    quintptr iptr = index.data(Qt::UserRole).toULongLong();
    if (iptr == 0) return false;
    RamObject *itemObj = reinterpret_cast<RamObject*>(iptr);
    if (itemObj->objectType() != RamObject::Asset && itemObj->objectType() != RamObject::Shot) return true;
    RamItem *item = qobject_cast<RamItem*>(itemObj);
    if(!item) return true;

    // check users
    bool ok = false;

    for(int i = 0; i < m_users.count(); i++)
    {
        for (int j = 1; j < sourceModel()->columnCount(); j++)
        {
            // get step
            RamStep *s = step(j);
            if (!s) continue;
            if ( item->isUserAssigned(m_users.at(i), s) )
            {
                ok = true;
                break;
            }
        }
        if (ok) break;
    }

    if (!ok && m_showUnassigned) for (int j = 1; j < sourceModel()->columnCount(); j++)
    {
        // get step
        RamStep *s = step(j);
        if (!s) continue;
        if ( item->isUnassigned(s) )
        {
            ok = true;
            break;
        }
    }

    if (!ok) return false;

    ok = false;

    for(int i = 0; i < m_states.count(); i++)
    {
        for (int j = 1; j < sourceModel()->columnCount(); j++)
        {
            // get step
            RamStep *s = step(j);
            if (!s) continue;
            if ( item->hasState(m_states.at(i), s) )
            {
                ok = true;
                break;
            }
        }
        if (ok) break;
    }

    return ok;
}

bool RamItemFilterModel::filterAcceptsColumn(int sourceColumn, const QModelIndex &sourceParent) const
{
    Q_UNUSED(sourceParent);

    if (sourceColumn == 0) return true;

    if ( step( sourceColumn ) ) return true;
    return false;
}

RamStep *RamItemFilterModel::step(int column) const
{
    if (column == 0) return nullptr;

    quintptr iptr = sourceModel()->headerData( column, Qt::Horizontal, Qt::UserRole).toULongLong();
    if (iptr == 0) return nullptr;
    RamStep *step = reinterpret_cast<RamStep*>(iptr);

    if (m_hiddenSteps.contains( step )) return nullptr;
    if (m_stepType == RamStep::All) return step;
    if( step->type() == m_stepType) return step;
    return nullptr;
}
