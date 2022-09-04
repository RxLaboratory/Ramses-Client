#include "ramobjectmodel.h"

RamObjectModel::RamObjectModel(RamAbstractObject::ObjectType type, QObject *parent)
    : QAbstractTableModel{parent}
{
    m_type = type;
}

int RamObjectModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_objectsUuids.count();
}

int RamObjectModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 1 + m_columnObjectsUuids.count();
}

QVariant RamObjectModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    int col = index.column();

    if (col == 0)
    {
        QString uuid = m_objectsUuids.at(row);
        return objectRoleData(uuid, role);
    }

    // TODO What should we return for other columns?
    return "";
}

QVariant RamObjectModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(role);

    if (orientation == Qt::Vertical)
    {
        QString uuid = m_objectsUuids.at(section);
        return objectRoleData(uuid, role);
    }
    else
    {
        QString uuid = m_objectsUuids.at(section - 1);
        return objectRoleData(uuid, role);
    }
}

void RamObjectModel::insertObjects(int row, QStringList uuids)
{
    beginInsertRows(QModelIndex(), row, row+uuids.count()-1);

    for (int i = uuids.count()-1; i >= 0; i--)
    {
        QString uuid = uuids.at(i);
        m_objectsUuids.insert(row, uuid);
        connectObject( uuid );
    }

    endInsertRows();
}

void RamObjectModel::removeObjects(QStringList uuids)
{
    // TODO maybe group calls to batch remove contiguous rows
    // if there are performance issues
    // beginRemoveRows can take a group of rows
    while(!uuids.isEmpty())
    {
        QString uuid = uuids.takeLast();
        int i = m_objectsUuids.indexOf(uuid);
        if (i>=0) disconnectObject(uuid);
        while( i >= 0 )
        {
            beginRemoveRows(QModelIndex(), i, i);
            m_objectsUuids.removeAt(i);
            endRemoveRows();
        }
    }
}

void RamObjectModel::insertColumnObjects(int column, QStringList uuids)
{
    beginInsertColumns(QModelIndex(), column+1, column+uuids.count());

    for (int i = uuids.count()-1; i >= 0; i--)
    {
        QString uuid = uuids.at(i);
        m_columnObjectsUuids.insert(column, uuid);
        connectObject( uuid );
    }

    endInsertColumns();
}

void RamObjectModel::removeColumnObjects(QStringList uuids)
{
    // TODO maybe group calls to batch remove contiguous rows
    // if there are performance issues
    // beginRemoveRows can take a group of rows
    while(!uuids.isEmpty())
    {
        QString uuid = uuids.takeLast();
        int i = m_columnObjectsUuids.indexOf(uuid);
        if (i>=0) disconnectObject(uuid);
        while( i >= 0 )
        {
            beginRemoveRows(QModelIndex(), i+1, i+1);
            m_columnObjectsUuids.removeAt(i);
            endRemoveRows();
        }
    }
}

bool RamObjectModel::moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild)
{
    Q_UNUSED(sourceParent)
    Q_UNUSED(destinationParent)

    int sourceEnd = sourceRow + count-1;
    int d = destinationChild;
    if (sourceRow < destinationChild) d++;

    if (!beginMoveRows(QModelIndex(), sourceRow, sourceEnd, QModelIndex(), d))
        return false;

    for (int i = 0; i < count ; i++)
    {
        if (destinationChild < sourceRow) m_objectsUuids.move(sourceEnd, destinationChild);
        else m_objectsUuids.move(sourceRow, destinationChild);
    }

    endMoveRows();

    return true;
}

RamObject *RamObjectModel::get(int row)
{
    return get(index(row, 0));
}

RamObject *RamObjectModel::get(QModelIndex index) const
{
    QString uuid = index.data(Qt::UserRole).toString();
    if (uuid == "") return nullptr;
    return RamObject::get(uuid, m_type);
}

RamObject *RamObjectModel::search(QString searchString) const
{
    // Shortname first
    for (int i = 0; i < m_objectsUuids.count(); i++)
    {
        QString uuid = m_objectsUuids.at(i);
        RamObject *o = RamObject::get(uuid, m_type);
        if (!o) continue;
        if (o->shortName() == searchString) return o;
    }
    // Name after
    for (int i = 0; i < m_objectsUuids.count(); i++)
    {
        QString uuid = m_objectsUuids.at(i);
        RamObject *o = RamObject::get(uuid, m_type);
        if (!o) continue;
        if (o->name() == searchString) return o;
    }
    return nullptr;
}

void RamObjectModel::objectDataChanged(RamObject *obj)
{
    QString uuid = obj->uuid();
    // Get the coordinates
    int row = m_objectsUuids.indexOf(uuid);
    if (row >= 0 && row < m_objectsUuids.count())
    {
        QModelIndex i = index(row, 0);
        emit dataChanged(i, i);
        emit headerDataChanged(Qt::Vertical, row, row);
        return;
    }
    int col = m_columnObjectsUuids.indexOf(uuid);
    if (col >= 0 && col < m_columnObjectsUuids.count())
    {
        emit headerDataChanged(Qt::Horizontal, col, col);
        return;
    }
    // TODO for columns
}

RamObject *RamObjectModel::getObject(QString uuid) const
{
    return RamObject::get(uuid, m_type);
}

void RamObjectModel::connectObject(QString uuid)
{
    RamObject *obj = getObject( uuid );
    connect(obj, &RamObject::dataChanged, this, &RamObjectModel::objectDataChanged);
}

void RamObjectModel::disconnectObject(QString uuid)
{
    RamObject *obj = getObject( uuid );
    disconnect(obj, nullptr, this, nullptr);
}

QVariant RamObjectModel::objectRoleData(QString uuid, int role) const
{
    // Implement base object data
    RamObject *obj = getObject(uuid);
    if(!obj) return uuid;
    return obj->roleData( role );
}
