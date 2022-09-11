#include "ramobjectmodel.h"
#include "ramstatus.h"
#include "ramstep.h"
#include "ramabstractitem.h"

RamObjectModel *RamObjectModel::m_emptyModel = nullptr;

RamObjectModel *RamObjectModel::emptyModel()
{
    if (!m_emptyModel) m_emptyModel = new RamObjectModel(RamObject::Object);
    return m_emptyModel;
}

RamObjectModel::RamObjectModel(RamAbstractObject::ObjectType type, QObject *parent)
    : QAbstractTableModel{parent}
{
    m_type = type;
    m_columnObjects = new RamObjectSortFilterProxyModel();
    connect(m_columnObjects, &RamObjectModel::rowsInserted, this, &RamObjectModel::insertModelColumns);
    connect(m_columnObjects, &RamObjectModel::rowsAboutToBeMoved, this, &RamObjectModel::removeModelColumns);
    connect(m_columnObjects, &RamObjectModel::rowsMoved, this, &RamObjectModel::moveModelColumns);
}

int RamObjectModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_objectsUuids.count();
}

int RamObjectModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 1 + m_columnObjects->rowCount();
}

QVariant RamObjectModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    int col = index.column();

    QString uuid = m_objectsUuids.at(row);

    if (col == 0) return objectRoleData(uuid, role);

    // For other columns
    // Get the object
    RamObject *obj = getObject(uuid);
    if (!obj) return uuid;
    QString colUuid = m_columnObjects->data(
                m_columnObjects->index(col-1, 0),
                RamObject::UUID
                ).toString();
    RamObject *colObj = obj->objectForColumn(colUuid);
    if (!colObj) {
        // Return info from a nostatus
        RamAbstractItem *item = qobject_cast<RamAbstractItem*>( obj );
        if (!item) return QVariant();
        QString stepUuid = headerData(col, Qt::Horizontal, RamObject::UUID).toString();
        if (stepUuid == "") return QVariant();
        RamStep *step = RamStep::get( stepUuid );
        if (!step) return QVariant();
        RamStatus *status = RamStatus::noStatus(item, step);
        if (status) return status->roleData(role);
        return QVariant();
    }
    return colObj->roleData(role);
}

QVariant RamObjectModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (section < 0) return QVariant();
    if (orientation == Qt::Horizontal && section >= columnCount()) return QVariant();
    if (orientation == Qt::Vertical && section >= rowCount()) return QVariant();

    // Vertical
    if (orientation == Qt::Vertical)
    {
        QString uuid = m_objectsUuids.at(section);
        if (role == Qt::DisplayRole) return objectRoleData(uuid, RamObject::ShortName);
        if (role == RamObject::UUID) return uuid;
        return QAbstractTableModel::headerData(section, orientation, role);
    }

    // Horizontal
    if (section > 0)
    {
        return m_columnObjects->headerData(section-1, Qt::Vertical, role);
    }

    // Horizontal first column
    return QVariant();
}

void RamObjectModel::insertObjects(int row, QStringList uuids)
{
    beginInsertRows(QModelIndex(), row, row + uuids.count()-1);

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
            i = m_objectsUuids.indexOf(uuid);
        }
    }
}

void RamObjectModel::setColumnModel(RamObjectModel *model)
{
    beginResetModel();
    m_columnObjects->setSourceModel(model);
    endResetModel();
}

RamObjectSortFilterProxyModel *RamObjectModel::columnModel() const
{
    return m_columnObjects;
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

void RamObjectModel::clear()
{
    if (rowCount() == 0) return;

    beginResetModel();

    while(m_objectsUuids.count() > 0)
    {
        disconnectObject( m_objectsUuids.takeLast() );
    }

    endResetModel();
}

void RamObjectModel::appendObject(QString uuid)
{
    if (m_objectsUuids.contains(uuid)) return;
    insertObjects(
                rowCount(),
                QStringList(uuid)
                );
}

RamObject *RamObjectModel::get(int row)
{
    return get(index(row, 0));
}

RamObject *RamObjectModel::get(QModelIndex index) const
{
    QString uuid = index.data(RamObject::UUID).toString();
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

bool RamObjectModel::contains(QString uuid) const
{
    return m_objectsUuids.contains(uuid);
}

RamObject::ObjectType RamObjectModel::type() const
{
    return m_type;
}

QStringList RamObjectModel::toStringList() const
{
    return m_objectsUuids;
}

void RamObjectModel::objectDataChanged(RamObject *obj)
{
    QString uuid = obj->uuid();
    // Get the coordinates
    int row = m_objectsUuids.indexOf(uuid);
    if (row >= 0 && row < m_objectsUuids.count())
    {
        QModelIndex i = index(row, 0);
        QModelIndex iEnd = index(row, columnCount() -1);
        emit dataChanged(i, iEnd);
        emit headerDataChanged(Qt::Vertical, row, row);
        return;
    }
}

void RamObjectModel::columnDataChanged(RamObject *obj)
{
    for (int i = 0; i < m_columnObjects->rowCount(); i++)
    {
        if (obj->is( m_columnObjects->get(i)))
        {
            int col = i+1;
            QModelIndex iStart = index(0, col);
            QModelIndex iEnd = index(rowCount() - 1, col);
            emit dataChanged(iStart, iEnd);
            emit headerDataChanged(Qt::Horizontal, col, col);
            return;
        }
    }
}

void RamObjectModel::insertModelColumns(const QModelIndex &parent, int first, int last)
{
    beginInsertColumns(parent, first+1, last+1);
    for (int i = first; i <= last; i++) {
        RamObject *obj = m_columnObjects->get(i);
        connect(obj, &RamObject::dataChanged, this, &RamObjectModel::columnDataChanged);
    }
    endInsertColumns();
}

void RamObjectModel::removeModelColumns(const QModelIndex &parent, int first, int last)
{
    beginRemoveColumns(parent, first+1, last+1);
    for (int i = first; i <= last; i++) {
        RamObject *obj = m_columnObjects->get(i);
        disconnect(obj, nullptr, this, nullptr);
    }
    endRemoveColumns();
}

void RamObjectModel::moveModelColumns(const QModelIndex &parent, int start, int end, const QModelIndex &destination, int row)
{
    beginMoveColumns(parent, start+1, end+1, destination, row);
    endMoveColumns();
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
