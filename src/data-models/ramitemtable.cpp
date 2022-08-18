#include "ramitemtable.h"
#include "ramproject.h"
#include "ramstate.h"
#include "ramstatus.h"

// PUBLIC //

RamItemTable *RamItemTable::get(QString uuid)
{
    return c( RamObjectList::get(uuid, ItemTable) );
}

RamItemTable *RamItemTable::c(RamObjectList *o)
{
    return qobject_cast<RamItemTable*>(o);
}

RamItemTable::RamItemTable(QString shortName, QString name, ObjectType type, QObject *parent, DataListMode mode):
    RamObjectList(shortName, name, type, mode, parent, ItemTable)
{
    construct();
    connectEvents();
}

RamItemTable::RamItemTable(QString uuid, QObject *parent):
    RamObjectList(uuid, parent, ItemTable)
{
    construct();
    connectEvents();
}

int RamItemTable::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    if (!m_steps) return 0;
    return m_steps->rowCount() + 1;
}

QVariant RamItemTable::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    int col = index.column();

    if (row < 0 || col < 0)
    {
        if (role > Qt::UserRole) return INT32_MIN;
        return 0;
    }

    // Return
    if (col == 0)
        return RamObjectList::data(index, role);

    // Get the item
    RamItem *item = RamItem::c( m_objectList.at(row) );

    if (role == Qt::InitialSortOrderRole)
    {
        return row;
    }
    else if (role == ItemShortName)
    {
        return item->shortName();
    }
    else if (role == ItemName )
    {
        return item->name();
    }
    else if (role == Pointer )
    {
        return reinterpret_cast<quintptr>(item);
    }

    // Get the step
    RamStep *step = stepAt(col);

    // Get the status
    RamStatus *status = item->status(step);

    if (!status)
    {
        // If there's no status yet, return a no
        if (role == Qt::DisplayRole)
            return "Nothing to do";

        if (role == Qt::StatusTipRole)
            return "NO | Nothing to do";

        if (role == Qt::ToolTipRole)
            return "NO | Nothing to do";

        status = RamStatus::noStatus(item, step);
    }

    if (role == Qt::DisplayRole)
        return status->name();

    if (role == Qt::StatusTipRole)
    {
        QString tip = status->state()->shortName() % " | " % status->state()->name();
        int timeSpent = status->timeSpent()/3600;
        tip = tip % " (" % QString::number(status->completionRatio()) % "%) | " %
                QString::number(timeSpent) % " hours (" %
                QString::number( RamStatus::hoursToDays(timeSpent), 'f', 2) % " days)";
        return tip;
    }

    if (role == Qt::ToolTipRole)
    {
        int timeSpent = status->timeSpent()/3600;
        float est = 0;
        if (status->useAutoEstimation()) est = status->estimation();
        else est = status->goal();
        return QString(
                    status->state()->shortName() %
                    " | " %
                    status->state()->name() %
                    "\nCompletion: " %
                    QString::number( status->completionRatio() ) %
                    "%\nTime spent: " %
                    QString::number(timeSpent) % " hours (" %
                    QString::number( RamStatus::hoursToDays(timeSpent), 'f', 2) % " days)" %
                    " / " % QString::number( est, 'f', 2 ) % " days" %
                    "\n" %
                    status->comment() );
    }

    if (role == Difficulty) // Difficulty order
    {
        return status->difficulty();
    }
    else if (role == TimeSpent) // Time spent order
    {
        return status->timeSpent();
    }
    else if (role == Estimation) // Estimation order
    {
        if (status->useAutoEstimation()) return status->estimation();
        else return status->goal();
    }
    else if (role == Completion) // Completion order
    {
        return status->completionRatio();
    }
    else if (role == Pointer)
    {
        return reinterpret_cast<quintptr>(status);
    }

    return QVariant();
}

QVariant RamItemTable::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Vertical) return RamObjectList::headerData(section, orientation, role);

    if (section == 0)
    {
        if ( role == Qt::DisplayRole )
        {
            return name();
        }
        return QVariant();
    }

    if ( role == Qt::DisplayRole )
        return stepAt(section)->name();

    if ( role == Qt::ForegroundRole )
        return QBrush(stepAt(section)->color());

    if ( role == Pointer)
        return reinterpret_cast<quintptr>( stepAt(section) );

    return QAbstractTableModel::headerData(section, orientation, role);
}

// PRIVATE SLOTS //

void RamItemTable::insertStep(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent);

    //We're inserting new columns
    beginInsertColumns(QModelIndex(), first+1, last+1);
    // Finished!
    endInsertColumns();
}

void RamItemTable::removeStep(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent);
    // We're removing columns
    beginRemoveColumns(QModelIndex(), first+1, last+1);
    endRemoveColumns();
}

void RamItemTable::removeItem(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent)

    for (int i = first; i <= last; i++)
    {
        RamItem *item = RamItem::c( m_objectList.at(i) );
        Q_ASSERT(item);

        // Disonnect the item
        disconnect(item, nullptr, this, nullptr);
    }

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

    emit headerDataChanged(Qt::Horizontal, col,col);
}

void RamItemTable::inserted(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent)

    for (int i = first; i <= last; i++)
    {
        RamItem *item = RamItem::c( m_objectList.at(i) );
        Q_ASSERT(item);

        // Connect the item to monitor its status
        connect( item, SIGNAL(statusChanged(RamItem*,RamStep*)), this, SLOT(statusChanged(RamItem*,RamStep*)) );
    }

    // Update the list of steps
    if (m_steps) return;

    RamItem *item = RamItem::c( m_objectList.first() );
    Q_ASSERT(item);

    RamProject *p = item->project();
    Q_ASSERT(p);

    m_steps = p->steps();

    connect( m_steps, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(insertStep(QModelIndex,int,int)));
    connect( m_steps, SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)), this, SLOT(removeStep(QModelIndex,int,int)));
}

// PRIVATE //

void RamItemTable::construct()
{
    this->m_objectType = RamAbstractObject::ItemTable;
    this->setObjectName(this->objectTypeName());
}

void RamItemTable::connectEvents()
{
    connect( this, &RamItemTable::rowsInserted, this, &RamItemTable::inserted);
    connect( this, &RamItemTable::rowsAboutToBeRemoved, this, &RamItemTable::removeItem);
}

RamStep *RamItemTable::stepAt(int col) const
{
    if (!m_steps) return nullptr;
    quintptr stepIptr = m_steps->data(m_steps->index(col-1, 0), Qt::UserRole).toULongLong();
    RamStep *step = reinterpret_cast<RamStep*>( stepIptr );
    return step;
}

int RamItemTable::stepCol(RamStep *step) const
{
    if (!m_steps) return -1;
    for (int i = 0; i < m_steps->rowCount(); i++)
    {
        RamObject *obj = reinterpret_cast<RamObject*>( m_steps->data( m_steps->index(i, 0), Qt::UserRole).toULongLong() );
        if (obj->is(step)) return i+1;
    }
    return -1;
}
