#include "ramitemtable.h"
#include "ramses.h"

// STATIC //

RamItemTable *RamItemTable::getObject(QString uuid, bool constructNew)
{
    RamObjectList<RamItem*> *obj = RamObjectList<RamItem*>::getObject(uuid);
    if (!obj && constructNew) return new RamItemTable(uuid);
    return static_cast<RamItemTable*>( obj ) ;
}

// PUBLIC //

RamItemTable::RamItemTable(QString shortName, QString name, RamObjectList<RamStep *> *steps, QObject *parent):
    RamObjectList<RamItem*>(shortName, name, parent)
{
    construct();
    m_steps = steps;
    this->insertData("steps", steps->uuid());
    connectEvents();
}

int RamItemTable::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
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
        return RamObjectList<RamItem*>::data(index, role);

    // Get the item
    RamItem *item = RamObjectList<RamItem*>::m_objectList.at(row);

    if (role == Qt::InitialSortOrderRole)
    {
        return RamObjectList<RamItem*>::objRow(item);
    }
    if ( role == Qt::UserRole + 1) // Default order
    {
        return RamObjectList<RamItem*>::objRow(item);
    }
    else if (role == Qt::UserRole + 2) // Short name order
    {
        return item->shortName();
    }
    else if (role == Qt::UserRole + 3 ) // Name order
    {
        return item->name();
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

        status = new RamStatus(
                        Ramses::instance()->ramUser(),
                        Ramses::instance()->noState(),
                        step,
                        item,
                        false);
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

    if (role == Qt::UserRole + 4) // Difficulty order
    {
        return status->difficulty();
    }
    else if (role == Qt::UserRole + 5) // Time spent order
    {
        return status->timeSpent();
    }
    else if (role == Qt::UserRole + 6) // Estimation order
    {
        if (status->useAutoEstimation()) return status->estimation();
        else return status->goal();
    }
    else if (role == Qt::UserRole + 7) // Completion order
    {
        return status->completionRatio();
    }

    quintptr iptr = reinterpret_cast<quintptr>(status);
    return iptr;
}

QVariant RamItemTable::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Vertical) return RamObjectList<RamItem*>::headerData(section, orientation, role);

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

    if ( role == Qt::UserRole)
        return reinterpret_cast<quintptr>( stepAt(section) );

    return QAbstractTableModel::headerData(section, orientation, role);
}

void RamItemTable::insertObject(int i, RamItem *item)
{
    if (contains(item)) return;

    // insert only valid items
    if (!item) return;

    // We're inserting rows
    beginInsertRows(QModelIndex(), i, i);

    m_objectList.insert(i , item);
    m_objects[item->uuid()] = item;
    connectObject(item);
    connectItem(item);

    endInsertRows();

    emit headerDataChanged(Qt::Horizontal, 0, m_steps->rowCount());
}

// PROTECTED //

RamItemTable::RamItemTable(QString uuid, QObject *parent):
    RamObjectList<RamItem*>(uuid, parent)
{
    construct();
    // Populate the list of steps
    QJsonObject d = RamAbstractObject::data();
    m_steps = RamObjectList<RamStep*>::getObject( d.value("steps").toString(), true );
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

// PRIVATE //

void RamItemTable::construct()
{
    this->m_objectType = RamAbstractObject::ItemTable;
    this->setObjectName(this->objectTypeName());
}

void RamItemTable::connectEvents()
{
    connect( m_steps, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(insertStep(QModelIndex,int,int)));
    connect( m_steps, SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)), this, SLOT(removeStep(QModelIndex,int,int)));
}

void RamItemTable::connectItem(RamItem *item)
{
    connect( item, SIGNAL(statusChanged(RamItem*,RamStep*)), this, SLOT(statusChanged(RamItem*,RamStep*)) );
}

RamStep *RamItemTable::stepAt(int col) const
{
    quintptr stepIptr = m_steps->data(m_steps->index(col-1, 0), Qt::UserRole).toULongLong();
    RamStep *step = reinterpret_cast<RamStep*>( stepIptr );
    return step;
}

int RamItemTable::stepCol(RamStep *step) const
{
    for (int i = 0; i < m_steps->rowCount(); i++)
    {
        RamObject *obj = reinterpret_cast<RamObject*>( m_steps->data( m_steps->index(i, 0), Qt::UserRole).toULongLong() );
        if (obj->is(step)) return i+1;
    }
    return -1;
}
