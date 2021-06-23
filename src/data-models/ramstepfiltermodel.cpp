#include "ramstepfiltermodel.h"

RamStepFilterModel::RamStepFilterModel(RamStep::Type stepType, QObject *parent) : QSortFilterProxyModel(parent)
{
    m_stepType = stepType;
}

void RamStepFilterModel::setList(RamObjectList *list)
{
    m_objectList = list;
    if(!list) return;
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
    Q_UNUSED(sourceParent)

    RamStep *step = qobject_cast<RamStep*>( m_objectList->at(sourceRow) );
    if(m_ignoreUuids.contains(step->uuid())) return false;
    return step->type() == m_stepType;
}
