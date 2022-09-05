#include "ramobjectsortfilterproxymodel.h"

#include "ramobjectmodel.h"

RamObjectSortFilterProxyModel::RamObjectSortFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel{parent}
{

}

RamObjectSortFilterProxyModel::RamObjectSortFilterProxyModel(QString listName, QObject *parent)
    : QSortFilterProxyModel{parent}
{
    m_listName = listName;
}

void RamObjectSortFilterProxyModel::setFilterList()
{
    m_isFilterList = true;
}

void RamObjectSortFilterProxyModel::setSingleColumn(bool singleColumn)
{
    m_isSingleColumn = singleColumn;
}

int RamObjectSortFilterProxyModel::rowCount(const QModelIndex &parent) const
{
    if (!sourceModel()) return 1;
    if (m_isFilterList) return sourceModel()->rowCount(parent) + 1;
    return sourceModel()->rowCount();
}

int RamObjectSortFilterProxyModel::columnCount(const QModelIndex &parent) const
{
    if (m_isSingleColumn) return 1;
    return QSortFilterProxyModel::columnCount(parent);
}

QVariant RamObjectSortFilterProxyModel::data(const QModelIndex &index, int role) const
{
    if (columnCount() == 0) return QVariant();

#if QT_VERSION > QT_VERSION_CHECK(5, 11, 0)
   if (!checkIndex(index, CheckIndexOption::IndexIsValid))
            return QVariant();
#endif

    // return ALL
    if (index.row() == 0)
    {
        if (role == Qt::DisplayRole) return "All " + m_listName;
        if (role == Qt::StatusTipRole) return m_listName;
        if (role == Qt::ToolTipRole) return "Do not filter " + m_listName;
        return 0;
    }

    return QSortFilterProxyModel::data( createIndex(index.row(),index.column()), role);
}

QModelIndex RamObjectSortFilterProxyModel::mapFromSource(const QModelIndex &sourceIndex) const
{
    if (!sourceIndex.isValid())
            return QModelIndex();

    if (sourceIndex.parent().isValid())
        return QModelIndex();

    return createIndex(sourceIndex.row()+1, sourceIndex.column());
}

QModelIndex RamObjectSortFilterProxyModel::mapToSource(const QModelIndex &proxyIndex) const
{
    if (!proxyIndex.isValid())
        return QModelIndex();

    if (proxyIndex.row() == 0)
        return QModelIndex();

    return sourceModel()->index(proxyIndex.row() - 1, proxyIndex.column());
}

Qt::ItemFlags RamObjectSortFilterProxyModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
         return Qt::NoItemFlags;
     if (index.row() == 0)
         return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
     return QSortFilterProxyModel::flags(createIndex(index.row(),index.column()));
}

QModelIndex RamObjectSortFilterProxyModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    if (row > rowCount())
            return QModelIndex();
    return createIndex(row, column);
}

QModelIndex RamObjectSortFilterProxyModel::parent(const QModelIndex &child) const
{
    Q_UNUSED(child)

    return QModelIndex();
}

RamObject *RamObjectSortFilterProxyModel::get(int row) const
{
    return get(index(row, 0));
}

RamObject *RamObjectSortFilterProxyModel::get(QModelIndex index) const
{
    QString uuid = index.data(Qt::UserRole).toString();
    if (uuid == "") return nullptr;

    RamObject::ObjectType t = type();
    return RamObject::get(uuid, t);
}

RamObject *RamObjectSortFilterProxyModel::searchObject(QString searchString) const
{
    // Shortname first
    for (int i = 0; i < rowCount(); i++)
    {
        RamObject *o = get(i);
        if (!o) continue;
        if (o->shortName() == searchString) return o;
    }
    // Name after
    for (int i = 0; i < rowCount(); i++)
    {
        RamObject *o = get(i);
        if (!o) continue;
        if (o->name() == searchString) return o;
    }
    return nullptr;
}

RamAbstractObject::ObjectType RamObjectSortFilterProxyModel::type() const
{
    QAbstractItemModel *sm = sourceModel();
    qDebug() << sm;
    RamObjectModel *model = qobject_cast<RamObjectModel*>( sourceModel() );
     qDebug() << model;
    if (!model) return RamObject::Object;

    return model->type();
}

void RamObjectSortFilterProxyModel::setFilterUuid(const QString &filterUuid)
{
    m_currentFilterUuid = filterUuid;
}

void RamObjectSortFilterProxyModel::search(const QString &searchStr)
{
    m_searchString = searchStr;
}

bool RamObjectSortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    Q_UNUSED(sourceParent)

    RamObjectModel *model = qobject_cast<RamObjectModel*>(sourceModel());
    RamObject *obj = model->get(sourceRow);
    if (!obj) return false;

    bool filterOK = m_currentFilterUuid == "" || obj->filterUuid() == m_currentFilterUuid;
    if (!filterOK) return false;
    if (m_searchString == "") return true;
    if (obj->shortName().contains(m_searchString, Qt::CaseInsensitive)) return true;
    return obj->name().contains(m_searchString, Qt::CaseInsensitive);
}

void RamObjectSortFilterProxyModel::prepareFilter()
{
    emit aboutToFilter();
    invalidateFilter();
}
