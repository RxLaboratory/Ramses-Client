#include "ramobjectlist.h"

#include "data-models/ramitemtable.h"
#include "ramses.h"
#include "ramshot.h"

// STATIC //

RamObjectList *RamObjectList::m_emptyList = nullptr;

RamObjectList *RamObjectList::get(QString uuid, ObjectType type)
{
    RamAbstractObject *obj = RamAbstractObject::get(uuid);
    if (obj) return static_cast<RamObjectList*>( obj ) ;

    switch(type)
    {
    case ItemTable: return new RamItemTable(uuid);
    case StepStatusHistory: return new RamStepStatusHistory(uuid);
    default: return new RamObjectList(uuid);
    }
}

RamObjectList *RamObjectList::c(QObject *obj)
{
    return qobject_cast<RamObjectList*>(obj);
}

RamObjectList *RamObjectList::emptyList()
{
    if (m_emptyList) m_emptyList = new RamObjectList("emptylist", "Empty", Object, Temp);
    return m_emptyList;
}

RamObject *RamObjectList::at(QModelIndex i)
{
    quintptr iptr = i.data(Pointer).toULongLong();
    if (iptr == 0) return nullptr;
    return reinterpret_cast<RamObject *>(iptr);
}

// PUBLIC //

RamObjectList::RamObjectList(QString shortName, QString name, ObjectType type, DataListMode mode, QObject *parent, ObjectType listType):
    QAbstractTableModel(parent),
    RamAbstractObject(shortName, name, listType, mode != ListObject)
{
    construct(parent);
    m_dataMode = mode;
    m_contentType = type;

    if (mode == ListObject)
    {
        insertData("type", objectTypeName( m_contentType ));
    }
    else if (mode == Table)
    {
        connect(DBInterface::instance(), &DBInterface::dataReset, this, &RamObjectList::reload);
    }
}

RamObjectList::RamObjectList(QString uuid, QObject *parent, ObjectType listType):
    QAbstractTableModel(parent),
    RamAbstractObject(uuid, listType)
{
    construct(parent);
    m_dataMode = ListObject;
    if (uuid.toLower() == "emptylist")
    {
        m_dataMode = Temp;
        return;
    }

    m_contentType = objectTypeFromName( getData("type").toString() );

    QJsonObject d = RamAbstractObject::data();
    // Populate the list
    QJsonArray arr = d.value("list").toArray();
    for (int i = 0; i < arr.count(); i++)
    {
        QString uuid = arr.at(i).toString();
        RamObject *obj = RamObject::get(uuid, m_contentType);
        addObj(obj);
    }
}

// QAbstractTableModel Reimplementation

int RamObjectList::rowCount(const QModelIndex & parent) const
{
    Q_UNUSED(parent);
    return m_objectList.count();
}

int RamObjectList::columnCount(const QModelIndex & parent) const
{
    Q_UNUSED(parent);
    return 1;
}

QVariant RamObjectList::data(const QModelIndex &index, int role) const
{
    // Invalid index
    int row = index.row();
    if (row < 0 || row >= m_objectList.count())
    {
        if (role == Qt::DisplayRole) return "Invalid index";
        if (role == Qt::StatusTipRole) return "Found an index which doesn't contain a valid object. That may be a bug.";
        if (role == Qt::ToolTipRole)  return "Found an index which doesn't contain a valid object. That may be a bug.";
        if (role == Pointer) return 0;
        return QVariant();
    }

    // Pass the pointer as an int to our delegate
    RamObject *obj = m_objectList.at(index.row());

    if (role == Qt::DisplayRole) return obj->name();

    if (role == Qt::StatusTipRole)
    {
        QString statusTip = QString(obj->shortName() % " | " % obj->name());
        // If it's a shot, let's add the duration
        if (obj->objectType() == RamAbstractObject::Shot)
        {
            RamShot *shot = RamShot::c(obj);
            statusTip += " | " % QString::number(shot->duration(), 'f', 2) % "s";
        }
        return statusTip;
    }

    if (role == Qt::ToolTipRole) {
        QString toopTip = obj->shortName() % " | " % obj->name();
        // If it's a shot, let's add the duration
        if (obj->objectType() == RamAbstractObject::Shot)
        {
            RamShot *shot = RamShot::c(obj);
            toopTip += "\n" % QString::number(shot->duration(), 'f', 2) % "s";
        }
        toopTip += "\n" % obj->comment();
        return  toopTip;
    }

    if (role == Pointer) return reinterpret_cast<quintptr>(obj);

    return QVariant();
}

QVariant RamObjectList::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Vertical)
    {
        // Invalid index
        if (section < 0 || section >= m_objectList.count())
        {
#ifdef QT_DEBUG
            if (role == Qt::DisplayRole) return "Invalid";
#endif
            return QVariant();
        }

        if ( role == Qt::DisplayRole )
            return m_objectList.at( section )->shortName();
        if ( role == Qt::UserRole )
            return m_objectList.at( section )->uuid();
    }
    return QAbstractTableModel::headerData(section, orientation, role);
}

QModelIndex RamObjectList::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return createIndex(row, column);
}

void RamObjectList::sort(int column, Qt::SortOrder order)
{
    Q_UNUSED(column)
    Q_UNUSED(order)

    std::sort(m_objectList.begin(), m_objectList.end(), objectSorter);
}

// List editing

void RamObjectList::clear(bool removeObjects)
{
    beginResetModel();

    // disconnect and remove objects
    for (int i = 0; i < m_objectList.count(); i++)
    {
        RamObject *o = m_objectList.at(i);
        if (removeObjects) o->remove();
        disconnect(o, nullptr, this, nullptr);
    }

    m_objectList.clear();
    m_objects.clear();

    endResetModel();
}

void RamObjectList::insertObject(int i, RamObject *obj)
{
    if (contains(obj)) return;

    beginInsertRows(QModelIndex(), i, i);

    addObj(obj, i);

    // Save data
    saveData();

    endInsertRows();
}

void RamObjectList::append(RamObject *obj)
{
    if (!obj) return;
    if (contains(obj)) return;

    insertObject(this->m_objectList.count(), obj);
}

QList<RamObject *> RamObjectList::removeIndices(QModelIndexList indices)
{
    std::sort(indices.begin(), indices.end(), indexSorter);

    QList<RamObject *> objs;

    for( int i = indices.count() -1; i >= 0; i--)
    {
        QModelIndex index = indices.at(i);
        quintptr iptr = index.data(Pointer).toULongLong();
        RamObject *o = reinterpret_cast<RamObject*>( iptr );
        objs << o;
        removeAll(o);
    }
    return objs;
}

RamObject *RamObjectList::takeObject(int i)
{
    beginRemoveRows(QModelIndex(), i, i);

    // take from list
    RamObject *obj = m_objectList.takeAt(i);
    disconnect(obj, nullptr, this, nullptr);

    // remove from map
    if( m_objects.contains(obj->uuid()) ) m_objects.remove(obj->uuid());

    saveData();

    endRemoveRows();
    return obj;
}

RamObject *RamObjectList::takeObject(QString uuid)
{
    // get from map
    RamObject *obj = m_objects.value(uuid, nullptr);
    if (!obj) return nullptr;

    // get index from list to remove
    int row = objRow(obj);
    if (row<0) return nullptr;

    return takeObject(row);

    return nullptr;
}

void RamObjectList::removeAll(QString uuid)
{
    // get from map
    RamObject *obj = m_objects.value(uuid, nullptr);
    if (!obj) return;

    int row = objRow(obj);
    if (row<0) return;

    takeObject(row);
}

QJsonObject RamObjectList::reloadData()
{
    QJsonObject d = RamAbstractObject::data();

    beginResetModel();

    // disconnect objects
    for (int i = 0; i < m_objectList.count(); i++)
    {
        RamObject *o = m_objectList.at(i);
        disconnect(o, nullptr, this, nullptr);
    }

    m_objectList.clear();
    m_objects.clear();

    // Reload
    QStringList uuids;
    if (m_dataMode == Table)
    {
        uuids = DBInterface::instance()->tableData( objectTypeName(m_contentType) );
    }
    else if (m_dataMode == ListObject)
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
        RamObject *o = RamObject::get( uuid, m_contentType );
        if (!o) continue;
        addObj(o);
    }

    endResetModel();

    return d;
}

void RamObjectList::saveData()
{
    switch(m_dataMode)
    {
    case ListObject:
    {
        // Convert to json array and save
        QJsonArray arr;
        for(int i = 0; i < m_objectList.count(); i++)
        {
            arr.append( m_objectList.at(i)->uuid() );
        }
        insertData("list", arr);

        break;
    }
    case Table:
    {
        break; // Nothing to do: no order, and creation/removal is handled by the objects themselves.
    }
    case Temp:
    {
        break;
    }
    }

    emit listChanged(this);
}

void RamObjectList::removeAll(RamObject *obj)
{
    int row = objRow(obj);
    if (row<0) return;

    takeObject(row);
}

// List information

bool RamObjectList::contains(RamObject *obj) const
{
    return contains(obj->uuid());
}

bool RamObjectList::contains(QString uuid) const
{
    return m_objects.contains( uuid );
}

RamObject *RamObjectList::fromUuid(QString uuid) const
{
    return m_objects.value(uuid, nullptr);
}

RamObject *RamObjectList::fromName(QString shortName, QString name) const
{
    for (int i = 0; i < m_objectList.count(); i++)
    {
        RamObject *o = m_objectList.at(i);
        if (o->shortName() == shortName)
        {
            if (name == "") return o;
            if (o->name() == name) return o;
        }
    }

    return nullptr;
}

RamObject *RamObjectList::at(int i) const
{
    if (i < 0) return nullptr;
    if (i > m_objectList.count() - 1) return nullptr;
    return m_objectList.at(i);
}

RamObject *RamObjectList::last() const
{
    return at(rowCount() - 1);
}

RamObject *RamObjectList::first() const
{
    return at(0);
}

QList<RamObject*> RamObjectList::toList()
{
    return m_objectList;
}

// PUBLIC SLOTS //

void RamObjectList::sort()
{
    sort(0);
}

void RamObjectList::reload()
{
    reloadData();
}

// PROTECTED //

void RamObjectList::connectObject(RamObject *obj)
{
    connect( obj, SIGNAL(removed(RamObject*)), this, SLOT(removeAll(RamObject*)) );
    connect( obj, &RamObject::dataChanged, this, &RamObjectList::objectChanged);
}

void RamObjectList::addObj(RamObject *o, int row)
{
    if (!o) return;
    if (contains(o)) return;

    if (row < 0) row = m_objectList.count();

    m_objectList.insert(row , o);
    m_objects[o->uuid()] = o;
    connectObject(o);
}

int RamObjectList::objRow(RamObject *obj) const
{
    for (int i = m_objectList.count() - 1; i >= 0; i--)
    {
        if ( m_objectList.at(i)->is(obj) ) return i;
    }
    return -1;
}

// PRIVATE SLOTS //

void RamObjectList::objectChanged(RamObject *obj)
{
    // Get row
    int row = objRow(obj);
    if (row < 0) return;
    if (row >= rowCount()) return;

    QModelIndex index = createIndex(row,0);
    emit dataChanged(index, index, {});
    emit headerDataChanged(Qt::Vertical, row, row);
}

// PRIVATE //

void RamObjectList::construct(QObject *parent)
{
    this->setObjectName(objectTypeName() + " | " + shortName() + "(" + m_uuid + ")");
    if (!parent) setParent(Ramses::instance());
}

void RamObjectList::connectEvents()
{
    connect(this, &QAbstractTableModel::rowsInserted, this, &RamObjectList::saveData);
    connect(this, &QAbstractTableModel::rowsRemoved, this, &RamObjectList::saveData);
    connect(this, &QAbstractTableModel::rowsMoved, this, &RamObjectList::saveData);
}

// SORTERS //

bool objectSorter(RamObject *a, RamObject *b)
{
    return a->shortName() < b->shortName();
}

bool indexSorter(QModelIndex a, QModelIndex b)
{
    return a.row() < b.row();
}
