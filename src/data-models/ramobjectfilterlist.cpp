#include "ramobjectfilterlist.h"

template<typename RO>
RamObjectFilterList<RO>::RamObjectFilterList(QObject *parent) : QSortFilterProxyModel(parent)
{

}

template<typename RO>
void RamObjectFilterList<RO>::setList(RamObjectList<RO> *list)
{
    m_objectList = list;
    if(!list) return;
    this->setSourceModel(list);
}

template<typename RO>
int RamObjectFilterList<RO>::rowCount(const QModelIndex &parent) const
{
    if (!m_objectList) return 1;
    return m_objectList->rowCount(parent) + 1;
}

template<typename RO>
QVariant RamObjectFilterList<RO>::data(const QModelIndex &index, int role) const
{
    if (columnCount() == 0) return QVariant();

#if QT_VERSION > QT_VERSION_CHECK(5, 11, 0)
   if (!checkIndex(index, CheckIndexOption::IndexIsValid))
            return QVariant();
#endif

    // return ALL
    if (index.row() == 0)
    {
        if (role == Qt::DisplayRole) return "All " + m_objectList->name();
        if (role == Qt::StatusTipRole) return m_objectList->name();
        if (role == Qt::ToolTipRole) return "Do not filter " + m_objectList->name();
        return 0;
    }

    return QSortFilterProxyModel::data( createIndex(index.row(),index.column()), role);
}

template<typename RO>
QModelIndex RamObjectFilterList<RO>::mapFromSource(const QModelIndex &sourceIndex) const
{
    if (!sourceIndex.isValid())
            return QModelIndex();

    if (sourceIndex.parent().isValid())
        return QModelIndex();

    return createIndex(sourceIndex.row()+1, sourceIndex.column());
}

template<typename RO>
QModelIndex RamObjectFilterList<RO>::mapToSource(const QModelIndex &proxyIndex) const
{
    if (!proxyIndex.isValid())
        return QModelIndex();

    if (proxyIndex.row() == 0)
        return QModelIndex();

    return m_objectList->index(proxyIndex.row() - 1, proxyIndex.column());
}

template<typename RO>
Qt::ItemFlags RamObjectFilterList<RO>::flags(const QModelIndex &index) const
{
    if (!index.isValid())
         return Qt::NoItemFlags;
     if (index.row() == 0)
         return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
     return QSortFilterProxyModel::flags(createIndex(index.row(),index.column()));
}

template<typename RO>
QModelIndex RamObjectFilterList<RO>::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    if (row > rowCount())
            return QModelIndex();
    return createIndex(row, column);
}

template<typename RO>
QModelIndex RamObjectFilterList<RO>::parent(const QModelIndex &child) const
{
    Q_UNUSED(child)

    return QModelIndex();
}
