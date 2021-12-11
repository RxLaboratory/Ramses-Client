#include "ramitemfiltermodel.h"

RamItemFilterModel::RamItemFilterModel(QObject *parent) : RamObjectFilterModel(parent)
{

}

void RamItemFilterModel::freeze()
{
    m_frozen = true;
}

void RamItemFilterModel::unFreeze()
{
    m_frozen = false;
    invalidateFilter();
}

void RamItemFilterModel::useFilters(bool use)
{
    m_userFilters = use;
}

void RamItemFilterModel::hideUser(RamUser *u)
{
    m_users.removeAll(u);
    if (!m_frozen) invalidateFilter();
}

void RamItemFilterModel::showUser(RamUser *u)
{
    if (!m_users.contains(u)) m_users << u;
    if (!m_frozen) invalidateFilter();
}

void RamItemFilterModel::clearUsers()
{
    m_users.clear();
    if (!m_frozen) invalidateFilter();
}

void RamItemFilterModel::showUnassigned(bool show)
{
    m_showUnassigned = show;
    if (!m_frozen) invalidateFilter();
}

void RamItemFilterModel::hideState(RamState *s)
{
    m_states.removeAll(s);
    if (!m_frozen) invalidateFilter();
}

void RamItemFilterModel::showState(RamState *s)
{
    if (!m_states.contains(s))
    {
        m_states << s;
        if (!m_frozen) invalidateFilter();
    }
}

void RamItemFilterModel::clearStates()
{
    m_states.clear();
    if (!m_frozen) invalidateFilter();
}

void RamItemFilterModel::setStepType(RamStep::Type t)
{
    m_stepType = t;
    if (!m_frozen) invalidateFilter();
}

void RamItemFilterModel::hideStep(RamStep *s)
{
    if (!m_hiddenSteps.contains(s))
    {
        m_hiddenSteps << s;
        if (!m_frozen) invalidateFilter();
    }
}

void RamItemFilterModel::showStep(RamStep *s)
{
    m_hiddenSteps.removeAll(s);
    if (!m_frozen) invalidateFilter();
}

void RamItemFilterModel::showAllSteps()
{
    m_hiddenSteps.clear();
    if (!m_frozen) invalidateFilter();
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

RamItemFilterModel::SortMode RamItemFilterModel::sortMode() const
{
    return m_sortMode;
}

void RamItemFilterModel::setSortMode(SortMode newSortMode)
{
    m_sortMode = newSortMode;
    if (m_sortMode == Default) this->setSortRole(Qt::InitialSortOrderRole);
    else this->setSortRole(Qt::UserRole + m_sortMode);
}

void RamItemFilterModel::resort(int col, Qt::SortOrder order)
{
    setSortMode(m_sortMode);
    this->sort(col, order);
}

void RamItemFilterModel::unsort()
{
    this->setSortRole(Qt::InitialSortOrderRole);
    resort(0,Qt::AscendingOrder);
}
