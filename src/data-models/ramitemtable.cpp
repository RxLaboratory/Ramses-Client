#include "ramitemtable.h"
#include "ramses.h"

RamItemTable::RamItemTable(RamStep::Type productionType, RamObjectList *steps, QObject *parent) : RamObjectList(parent)
{
    m_productionType = productionType;
    m_steps = new RamStepFilterModel(productionType, this);
    m_steps->setList(steps);
    connectEvents();
}

RamItemTable::RamItemTable(RamStep::Type productionType, RamObjectList *steps, QString shortName, QString name, QObject *parent)
        : RamObjectList(shortName, name, parent)
{
    m_productionType = productionType;
    m_steps = new RamStepFilterModel(productionType, this);
    m_steps->setList(steps);
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

    // Return the item
    if (col == 0)
        return RamObjectList::data(index, role);

    // Get the item
    RamObject *itemObj = m_objectsList.at(row);
    RamItem *item = qobject_cast<RamItem*>( itemObj );

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
                        item);

        quintptr iptr = reinterpret_cast<quintptr>(status);
        return iptr;
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
        float estimation = status->estimation();
        if (estimation < 0) estimation = status->autoEstimation();
        return QString(
                    status->state()->shortName() %
                    " | " %
                    status->state()->name() %
                    "\nCompletion: " %
                    QString::number( status->completionRatio() ) %
                    "%\nTime spent: " %
                    QString::number(timeSpent) % " hours (" %
                    QString::number( RamStatus::hoursToDays(timeSpent), 'f', 2) % " days)" %
                    " / " % QString::number( estimation, 'f', 2 ) % " days" %
                    "\n" %
                    status->comment() );
    }

    quintptr iptr = reinterpret_cast<quintptr>(status);
    return iptr;
}

QVariant RamItemTable::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Vertical) return RamObjectList::headerData(section, orientation, role);

    if (section == 0)
    {
        if ( role == Qt::DisplayRole )
        {
            if (m_productionType == RamStep::AssetProduction) return "Assets";
            else if (m_productionType == RamStep::ShotProduction) return "Shots";
            else return "Items";
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

    emit headerDataChanged(Qt::Horizontal, 0, m_steps->rowCount());
}

void RamItemTable::addStepFilter(RamObject *stepObj)
{
    m_steps->ignoreUuid(stepObj->uuid());
}

void RamItemTable::removeStepFilter(RamObject *stepObj)
{
    m_steps->acceptUuid(stepObj->uuid());
}

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

void RamItemTable::connectEvents()
{
    connect( m_steps, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(insertStep(QModelIndex,int,int)));
    connect( m_steps, SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)), this, SLOT(removeStep(QModelIndex,int,int)));
}

void RamItemTable::connectItem(RamItem *item)
{
    QList<QMetaObject::Connection> c = m_connections[ item->uuid() ];
    c << connect( item, SIGNAL(statusChanged(RamItem*,RamStep*)), this, SLOT(statusChanged(RamItem*,RamStep*)) );
    m_connections[ item->uuid() ] << c;
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


