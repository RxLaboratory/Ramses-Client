#include "ramobjectlist.h"

#include "ramses.h"

// STATIC //

template<typename RO>
RamObjectList<RO> *RamObjectList<RO>::getObject(QString uuid, bool constructNew)
{
    RamAbstractObject *obj = RamAbstractObject::getObject(uuid);
    if (!obj && constructNew) return new RamObjectList(uuid);
    return static_cast<RamObjectList*>( obj ) ;
}

// PUBLIC //

template<typename RO>
RamObjectList<RO>::RamObjectList(QString shortName, QString name, QObject *parent, DataListMode mode):
    QAbstractTableModel(parent),
    RamAbstractObject(shortName, name, ObjectList)

{
    construct(parent);
    m_dataMode = mode;

    m_tableName = shortName;
}

// QAbstractTableModel Reimplementation

template<typename RO>
int RamObjectList<RO>::rowCount(const QModelIndex & parent) const
{
    Q_UNUSED(parent);
    return m_objectList.count();
}

template<typename RO>
int RamObjectList<RO>::columnCount(const QModelIndex & parent) const
{
    Q_UNUSED(parent);
    return 1;
}

template<typename RO>
QVariant RamObjectList<RO>::data(const QModelIndex &index, int role) const
{
    Q_UNUSED(role);

    // Pass the pointer as an int to our delegate
    RO obj = m_objectList.at(index.row());

    if (role == Qt::DisplayRole) return obj->name();

    if (role == Qt::StatusTipRole)
    {
        QString statusTip = QString(obj->shortName() % " | " % obj->name());
        // If it's a shot, let's add the duration
        if (obj->objectType() == RamAbstractObject::Shot)
        {
            statusTip += " | " % QString::number(obj->duration(), 'f', 2) % "s";
        }
        return statusTip;
    }

    if (role == Qt::ToolTipRole) {
        QString toopTip = obj->shortName() % " | " % obj->name();
        // If it's a shot, let's add the duration
        if (obj->objectType() == RamAbstractObject::Shot)
        {
            toopTip += "\n" % QString::number(obj->duration(), 'f', 2) % "s";
        }
        toopTip += "\n" % obj->comment();
        return  toopTip;
    }

    quintptr iptr = reinterpret_cast<quintptr>(obj);
    return iptr;
}

template<typename RO>
QVariant RamObjectList<RO>::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Vertical)
    {
        if ( role == Qt::DisplayRole )
            return m_objectList.at( section )->shortName();
        if ( role == Qt::UserRole )
            return m_objectList.at( section )->uuid();
    }
    return QAbstractTableModel::headerData(section, orientation, role);
}

template<typename RO>
void RamObjectList<RO>::sort(int column, Qt::SortOrder order)
{
    Q_UNUSED(column)
    Q_UNUSED(order)

    std::sort(m_objectList.begin(), m_objectList.end(), objectSorter);
}

// List editing

template<typename RO>
void RamObjectList<RO>::clear(bool removeObjects)
{
    beginResetModel();

    // disconnect and remove objects
    for (int i = 0; i < m_objectList.count(); i++)
    {
        RO o = m_objectList.at(i);
        if (removeObjects) o->remove();
        disconnect(o, nullptr, this, nullptr);
    }

    m_objectList.clear();
    m_objects.clear();

    endResetModel();
}

template<typename RO>
void RamObjectList<RO>::insertObject(int i, RO obj)
{
    if (contains(obj)) return;

    beginInsertRows(QModelIndex(), i, i);

    m_objectList.insert(i , obj);
    m_objects[obj->uuid()] = obj;
    connectObject(obj);

    endInsertRows();
}

template<typename RO>
void RamObjectList<RO>::append(RO obj)
{
    if (!obj) return;
    if (contains(obj)) return;

    insertObject(this->m_objectList.count(), obj);
}

template<typename RO>
QList<RO> RamObjectList<RO>::removeIndices(QModelIndexList indices)
{
    std::sort(indices.begin(), indices.end(), indexSorter);

    QList<RO> objs;

    for( int i = indices.count() -1; i >= 0; i--)
    {
        QModelIndex index = indices.at(i);
        quintptr iptr = index.data(Qt::UserRole).toULongLong();
        RO o = reinterpret_cast<RO>( iptr );
        objs << o;
        removeAll(o);
    }
    return objs;
}

template<typename RO>
RO RamObjectList<RO>::takeObject(int i)
{
    beginRemoveRows(QModelIndex(), i, i);

    // take from list
    RO obj = m_objectList.takeAt(i);
    disconnect(obj, nullptr, this, nullptr);

    // remove from map
    if( m_objects.contains(obj->uuid()) ) m_objects.remove(obj->uuid());

    endRemoveRows();
    return obj;
}

template<typename RO>
RO RamObjectList<RO>::takeObject(QString uuid)
{
    // get from map
    RO obj = m_objects.value(uuid, nullptr);
    if (!obj) return nullptr;

    // get index from list to remove
    int row = objRow(obj);
    if (row<0) return nullptr;

    return takeObject(row);

    return nullptr;
}

template<typename RO>
void RamObjectList<RO>::removeAll(QString uuid)
{
    // get from map
    RO obj = m_objects.value(uuid, nullptr);
    if (!obj) return;

    int row = objRow(obj);
    if (row<0) return;

    takeObject(row);
}

template<typename RO>
QJsonObject RamObjectList<RO>::reloadData()
{
    QJsonObject d = data();

    beginResetModel();

    // disconnect objects
    for (int i = 0; i < m_objectList.count(); i++)
    {
        RO o = m_objectList.at(i);
        disconnect(o, nullptr, this, nullptr);
    }

    m_objectList.clear();
    m_objects.clear();

    // Reload
    QStringList uuids;
    if (m_dataMode == Table)
    {
        uuids = DBInterface::instance()->tableData(m_tableName);
    }
    else if (m_dataMode == Object)
    {
        QJsonArray arr = d.value("list").toArray();
        for (int i = 0; i < arr.count(); i++)
        {
            uuids << arr[i].toString();
        }
    }

    for (int i = 0; i < uuids.count(); i++)
    {
        QString uuid = uuids.at(i);
        RO o = RO::getObject( uuid );
        m_objectList << o;
        m_objects[uuid] = o;
        connectObject(o);
    }

    endResetModel();

    return d;
}

template<typename RO>
void RamObjectList<RO>::removeAll(RO obj)
{
    int row = objRow(obj);
    if (row<0) return;

    takeObject(row);
}

// List information

template<typename RO>
bool RamObjectList<RO>::contains(RO obj) const
{
    return contains(obj->uuid());
}

template<typename RO>
bool RamObjectList<RO>::contains(QString uuid) const
{
    return m_objects.contains( uuid );
}

template<typename RO>
RO RamObjectList<RO>::fromUuid(QString uuid) const
{
    return m_objects.value(uuid, nullptr);
}

template<typename RO>
RO RamObjectList<RO>::fromName(QString shortName, QString name) const
{
    for (int i = 0; i < m_objectList.count(); i++)
    {
        RO o = m_objectList.at(i);
        if (o->shortName() == shortName)
        {
            if (name == "") return o;
            if (o->name() == name) return o;
        }
    }

    return nullptr;
}

template<typename RO>
RO RamObjectList<RO>::at(int i) const
{
    if (i < 0) return nullptr;
    if (i > m_objectList.count() - 1) return nullptr;
    return m_objectList.at(i);
}

template<typename RO>
RO RamObjectList<RO>::at(QModelIndex i) const
{
    return at(i.row());
}

template<typename RO>
QList<RO> RamObjectList<RO>::toList()
{
    return m_objectList;
}

// PUBLIC SLOTS //

template<typename RO>
void RamObjectList<RO>::sort()
{
    sort(0);
}

template<typename RO>
void RamObjectList<RO>::reload()
{
    reloadData();
}

// PROTECTED //

template<typename RO>
RamObjectList<RO>::RamObjectList(QString uuid, QObject *parent):
    QAbstractTableModel(parent),
    RamAbstractObject(uuid, ObjectList)
{
    construct();
    m_dataMode = Object;
    // Populate the list
    QJsonObject d = RamAbstractObject::data();
    QJsonArray arr = d.value("list").toArray();
    for (int i = 0; i < arr.count(); i++)
    {
        QString uuid = arr.at(i).toString();
        RO obj = RO::getObject(uuid, true);
        append(obj);
    }
}

template<typename RO>
void RamObjectList<RO>::connectObject(RO obj)
{
    connect( obj, SIGNAL(removed(RamObject*)), this, SLOT(removeAll(RamObject*)));
    connect( obj, SIGNAL(changed(RamObject*)), this, SLOT(objectChanged(RamObject*)));
}

template<typename RO>
int RamObjectList<RO>::objRow(RO obj) const
{
    for (int i = m_objectList.count() - 1; i >= 0; i--)
    {
        if ( m_objectList.at(i)->is(obj) ) return i;
    }
    return -1;
}

// PRIVATE SLOTS //

template<typename RO> void RamObjectList<RO>::objectChanged(RO obj)
{
    // Get row
    int row = objRow(obj);
    if (row < 0) return;
    if (row >= rowCount()) return;

    QModelIndex index = createIndex(row,0);
    emit dataChanged(index, index, {});
    emit headerDataChanged(Qt::Vertical, row, row);
}

template<typename RO>
void RamObjectList<RO>::listChanged()
{
    switch(m_dataMode)
    {
    case Object:
    {
        // Convert to json array and save
        QJsonArray arr;
        for(int i = 0; i < m_objectList.count(); i++)
        {
            arr.append( m_objectList.at(i)->uuid() );
        }
        insertData("list", arr);
        return;
    }
    case Table:
    {
        return; // Nothing to do: no order, and creation/removal is handled by the objects themselves.
    }
    case Temp:
    {
        return;
    }
    }
}

// PRIVATE //

template<typename RO>
void RamObjectList<RO>::construct(QObject *parent)
{
    this->setObjectName(objectTypeName());
    if (!parent) setParent(Ramses::instance());
}

template<typename RO> void RamObjectList<RO>::connectEvents()
{
    connect(this, &QAbstractTableModel::rowsInserted, this, &RamObjectList::listChanged);
    connect(this, &QAbstractTableModel::rowsRemoved, this, &RamObjectList::listChanged);
    connect(this, &QAbstractTableModel::rowsMoved, this, &RamObjectList::listChanged);
}

// SORTERS //

template<typename RO> bool objectSorter(RamObject *a, RamObject *b)
{
    return a->shortName() < b->shortName();
}

template<typename RO> bool indexSorter(QModelIndex a, QModelIndex b)
{
    return a.row() < b.row();
}
