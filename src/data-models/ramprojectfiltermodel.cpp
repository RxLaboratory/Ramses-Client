#include "ramprojectfiltermodel.h"

RamProjectFilterModel::RamProjectFilterModel(QObject *parent) : QSortFilterProxyModel(parent)
{
    RamObjectList *projects = Ramses::instance()->projects();
    this->setDynamicSortFilter(true);
    this->setSourceModel(projects);
    connect(projects, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(doInvalidateFilter()));
    connect(projects, SIGNAL(modelReset()), this, SLOT(doInvalidateFilter()));
}

void RamProjectFilterModel::addUser(QString uuid)
{
    if(!m_userUuids.contains(uuid))
    {
        m_userUuids << uuid;
        invalidateFilter();
    }
}

void RamProjectFilterModel::removeUser(QString uuid)
{
    m_userUuids.removeAll(uuid);
    invalidateFilter();
}

void RamProjectFilterModel::clearUsers()
{
    m_userUuids.clear();
    invalidateFilter();
}

bool RamProjectFilterModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);

    quintptr iptr = index.data(Qt::UserRole).toULongLong();
    if (iptr == 0) return false;
    RamProject *project = reinterpret_cast<RamProject*>(iptr);

    if ( m_userUuids.count() == 0 ) return true;

    for (int i = 0; i < m_userUuids.count(); i++)
    {
        if ( project->users()->contains(m_userUuids.at(i))) return true;
    }
    return false;
}

void RamProjectFilterModel::doInvalidateFilter()
{
    invalidateFilter();
}
