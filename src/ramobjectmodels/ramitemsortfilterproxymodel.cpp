#include "ramitemsortfilterproxymodel.h"

#include "ramproject.h"
#include "ramstep.h"
#include "ramabstractitem.h"

RamItemSortFilterProxyModel::RamItemSortFilterProxyModel(QObject *parent) : RamObjectSortFilterProxyModel(parent)
{

}

void RamItemSortFilterProxyModel::useFilters(bool use)
{
    m_userFilters = use;
}

void RamItemSortFilterProxyModel::hideUser(RamObject *u)
{
    m_users.removeAll(u);
    if (!m_frozen) prepareFilter();
}

void RamItemSortFilterProxyModel::showUser(RamObject *u)
{
    if (!m_users.contains(u)) m_users << u;
    if (!m_frozen) prepareFilter();
}

void RamItemSortFilterProxyModel::clearUsers()
{
    m_users.clear();
    if (!m_frozen) prepareFilter();
}

void RamItemSortFilterProxyModel::showUnassigned(bool show)
{
    m_showUnassigned = show;
    if (!m_frozen) prepareFilter();
}

void RamItemSortFilterProxyModel::hideState(RamObject *s)
{
    m_states.removeAll(s);
    if (!m_frozen) prepareFilter();
}

void RamItemSortFilterProxyModel::showState(RamObject *s)
{
    if (!m_states.contains(s))
    {
        m_states << s;
        if (!m_frozen) prepareFilter();
    }
}

void RamItemSortFilterProxyModel::clearStates()
{
    m_states.clear();
    if (!m_frozen) prepareFilter();
}

void RamItemSortFilterProxyModel::setStepType(RamStep::Type t)
{
    m_stepType = t;
    if (!m_frozen) prepareFilter();
}

void RamItemSortFilterProxyModel::hideStep(RamObject *s)
{
    if (!m_hiddenSteps.contains(s))
    {
        m_hiddenSteps << s;
        if (!m_frozen) prepareFilter();
    }
}

void RamItemSortFilterProxyModel::showStep(RamObject *s)
{
    m_hiddenSteps.removeAll(s);
    if (!m_frozen) prepareFilter();
}

void RamItemSortFilterProxyModel::showAllSteps()
{
    m_hiddenSteps.clear();
    if (!m_frozen) prepareFilter();
}

bool RamItemSortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    // If not accepted by the search/uuid filter, return
    if(! RamObjectSortFilterProxyModel::filterAcceptsRow(sourceRow, sourceParent)) return false;

    if (!m_userFilters) return true;

    QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);
    QString itemUuid = index.data(RamObject::UUID).toString();
    RamObject::ObjectType itemType = static_cast<RamObject::ObjectType>( index.data(RamObject::Type).toInt() );
    RamObject *itemObj = RamObject::get(itemUuid, itemType);
    if (itemObj->objectType() != RamObject::Asset && itemObj->objectType() != RamObject::Shot) return true;
    RamAbstractItem *item = qobject_cast<RamAbstractItem*>(itemObj);
    if(!item) return false;

    return true;

    RamProject *proj = item->project();
    if (!proj) return false;

    // check users
    bool ok = false;

    for (int i = 0; i < m_users.count(); i++)
    {
        ok = proj->isUserAssigned(m_users.at(i), item);
        if (ok) break;
    }

    if (!ok && m_showUnassigned)
    {
        ok = proj->isUnassigned(item);
    }

    if (!ok) return false;

    ok = false;

    // check states
    for (int i = 0; i < m_states.count(); i++)
    {
        ok = proj->hasState(m_states.at(i), item);
        if (ok) break;
    }

    return ok;
}

bool RamItemSortFilterProxyModel::filterAcceptsColumn(int sourceColumn, const QModelIndex &sourceParent) const
{
    Q_UNUSED(sourceParent);

    if (m_frozen) return false;

    if (sourceColumn == 0) return true;

    if ( step( sourceColumn ) ) return true;
    return false;
}

RamStep *RamItemSortFilterProxyModel::step(int column) const
{
    if (column == 0) return nullptr;

    QString stepUuid = sourceModel()->headerData( column, Qt::Horizontal, RamObject::UUID).toString();
    if (stepUuid == "") return nullptr;
    RamStep *step = RamStep::get(stepUuid);

    if (m_hiddenSteps.contains( step )) return nullptr;
    if( step->type() == m_stepType) return step;
    return nullptr;
}

int RamItemSortFilterProxyModel::sortMode() const
{
    return m_sortMode;
}

void RamItemSortFilterProxyModel::setSortMode(int newSortMode)
{
    m_sortMode = newSortMode;
    this->setSortRole(newSortMode);
    if (newSortMode == Qt::InitialSortOrderRole) this->sort(-1);
}

void RamItemSortFilterProxyModel::resort(int col, Qt::SortOrder order)
{
    setSortMode(m_sortMode);
    this->sort(col, order);
}

void RamItemSortFilterProxyModel::unsort()
{
    this->setSortRole(Qt::InitialSortOrderRole);
    resort(0,Qt::AscendingOrder);
}
