#include "ramstepfiltermodel.h"

RamStepFilterModel::RamStepFilterModel(RamStep::Type stepType, QObject *parent) : QSortFilterProxyModel(parent)
{
    m_emptyList = new RamObjectList();
    m_stepType = stepType;
    this->setSourceModel(m_emptyList);
}

void RamStepFilterModel::setList(RamObjectList *list)
{
    if(!list) this->setSourceModel(m_emptyList);
    this->setSourceModel(list);
}

void RamStepFilterModel::ignoreUuid(QString uuid)
{
    if (!m_ignoreUuids.contains(uuid))
    {
        m_ignoreUuids << uuid;
        invalidateFilter();
    }
}

void RamStepFilterModel::acceptUuid(QString uuid)
{
    m_ignoreUuids.removeAll(uuid);
    invalidateFilter();
}

bool RamStepFilterModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);

    quintptr iptr = index.data(Qt::UserRole).toULongLong();
    if (iptr == 0) return false;
    RamStep *step = reinterpret_cast<RamStep*>(iptr);

    if(m_ignoreUuids.contains(step->uuid())) return false;
    if (m_stepType == RamStep::All) return true;
    return step->type() == m_stepType;
}
