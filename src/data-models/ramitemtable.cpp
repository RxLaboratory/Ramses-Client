#include "ramitemtable.h"

RamItemTable::RamItemTable(RamStep::Type productionType, RamObjectList *steps, QObject *parent) : RamObjectList(parent)
{
    m_steps = steps;
    m_productionType = productionType;
}

RamItemTable::RamItemTable(RamStep::Type productionType, RamObjectList *steps, QString shortName, QString name, QObject *parent)
        : RamObjectList(shortName, name, parent)
{
    m_steps = steps;
    m_productionType = productionType;
}

int RamItemTable::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_steps->count() + 1;
}

QVariant RamItemTable::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    int col = index.column();

    // Return the item
    if (row == 0)
        return RamObjectList::data(index, role);

    // Get the item
    RamObject *itemObj = m_objectsList.at(row);
    RamItem *item = qobject_cast<RamItem*>( itemObj );
    if (!item) return 0;

    // Get the step
    RamObject *stepObj = m_steps->at( col - 1 );

    // Get the status
    RamStatus *status = item->status(stepObj);

    // If there's no status yet, just return 0
    if (!status)
        return 0;

    quintptr iptr = reinterpret_cast<quintptr>(status);
    return iptr;
}

void RamItemTable::insertObject(int i, RamObject *obj)
{
    if (contains(obj)) return;

    RamItem *item = qobject_cast<RamItem*>( obj );
    // insert only valid items
    if (!item) return;

    // We're inserting rows
    beginInsertRows(QModelIndex(), i, i);

    m_objectsList.insert(i , obj);
    m_objects[obj->uuid()] = obj;
    connectObject(obj);
    connectItem(item);

    endInsertRows();
}

void RamItemTable::insertStep(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent);

    //We're inserting new columns
    beginInsertColumns(QModelIndex(), first+1, last+1);

    // TODO Connect changed for headers

    // Finished!
    endInsertColumns();
}

void RamItemTable::removeStep(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent);

    // We're removing columns
    beginRemoveColumns(QModelIndex(), first+1, last+1);

    // TODO Disconnect changed

    endRemoveColumns();
}

void RamItemTable::statusChanged(RamItem *item, RamStep *step)
{
    // We just need to notify that the data has changed for the item/step

    int row = objRow( item );
    int col = stepCol( step );

    if (col < 0) return;
    if (row < 0) return;

    QModelIndex index = createIndex(row, col);

    emit dataChanged( index, index, {Qt::DisplayRole});
}

const RamStep::Type &RamItemTable::productionType() const
{
    return m_productionType;
}

void RamItemTable::connectEvents()
{
    connect( m_steps, SIGNAL(columnsInserted(const QModelIndex&,int,int)), this, SLOT(insertStep(const QModelIndex&,int,int)));
    connect( m_steps, SIGNAL(columnsAboutToBeRemoved(const QModelIndex&,int,int)), this, SLOT(removeStep(const QModelIndex&,int,int)));
}

void RamItemTable::connectItem(RamItem *item)
{
    QList<QMetaObject::Connection> c = m_connections[ item->uuid() ];
    c << connect( item, SIGNAL(statusChanged(RamItem*,RamStep*)), this, SLOT(statusChanged(RamItem*,RamStep*)) );
    m_connections[ item->uuid() ] << c;
}

int RamItemTable::stepCol(RamStep *step)
{
    for (int i = 0 ; i < m_steps->count(); i++)
    {
        if (step->is(m_steps->at(i))) return i+1;
    }
    return -1;
}


