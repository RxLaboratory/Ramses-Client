#include "ramstatustablemodel.h"

#include "ramstatus.h"
#include "ramasset.h"
#include "ramshot.h"

RamStatusTableModel::RamStatusTableModel(DBTableFilterProxyModel *steps, DBTableFilterProxyModel *items, QObject *parent)
    : QAbstractTableModel{parent}
{
    m_steps = steps;
    m_items = items;

    m_status = new DBTableModel(RamObject::Status, this);
    m_status->addFilterValues("step", m_steps->toStringList());
    m_status->addLookUpKey("item");
    m_status->addLookUpKey("step");
}

void RamStatusTableModel::load()
{
    if (m_loaded) return;
    m_loaded = true;

    m_steps->load();
    m_items->load();
    m_status->load();

    // Connect steps & items
    connect(m_steps, &DBTableModel::rowsInserted, this, &RamStatusTableModel::stepsInserted);
    connect(m_steps, &DBTableModel::rowsAboutToBeRemoved, this, &RamStatusTableModel::stepsRemoved);
    connect(m_steps, &DBTableModel::rowsMoved, this, &RamStatusTableModel::stepsMoved);
    connect(m_steps, &DBTableModel::dataChanged, this, &RamStatusTableModel::stepsDataChanged);

    connect(m_items, &DBTableModel::rowsInserted, this, &RamStatusTableModel::itemsInserted);
    connect(m_items, &DBTableModel::rowsAboutToBeRemoved, this, &RamStatusTableModel::itemsRemoved);
    connect(m_items, &DBTableModel::rowsMoved, this, &RamStatusTableModel::itemsMoved);
    connect(m_items, &DBTableModel::dataChanged, this, &RamStatusTableModel::itemsDataChanged);

    connect(m_status, &DBTableModel::dataChanged, this, &RamStatusTableModel::statusDataChanged);
}

int RamStatusTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_items->rowCount();
}

int RamStatusTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    // ADD 1: the item details column
    return m_steps->rowCount() + 1;
}

QVariant RamStatusTableModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    int column = index.column();

    // Handle wrong indices
    if (row < 0) return QVariant();
    if (row >= rowCount()) return QVariant();
    if (column < 0) return QVariant();
    if (column >= columnCount()) return QVariant();

    // First column: the item
    if (column == 0) return m_items->data( m_items->index(row, 0), role);

    // Other columns: get the (latest) status

    // Make sure status are loaded
    m_status->load();

    // Get the item
    QString itemUuid = m_items->getUuid(row);
    if (itemUuid == "") return QVariant();

    // Get the step
    // The first column is the item, so -1
    QString stepUuid = m_steps->getUuid(column - 1);
    if (stepUuid == "") return QVariant();

    RamStatus *status = getStatus(itemUuid, stepUuid);

    if (!status) return QVariant();

    if (role == RamObject::Pointer) return reinterpret_cast<quintptr>(status);
    return status->roleData(role);
}

QVariant RamStatusTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    // Handle wrong indices
    if (section < 0) return QVariant();
    if (orientation == Qt::Horizontal && section >= columnCount()) return QVariant();
    if (orientation == Qt::Vertical && section >= rowCount()) return QVariant();

    if (orientation == Qt::Horizontal)
    {
        // First column is empty
        if (section == 0) return QVariant();

        // Other columns are the steps
        return m_steps->data( m_steps->index(section-1, 0), role);
    }

    // Item data
    if (role == Qt::DisplayRole)
        return m_items->data( m_items->index(section, 0), RamObject::ShortName);
    if (role == RamObject::Pointer)
        return m_items->data( m_items->index(section, 0), RamObject::Pointer);
    if (role == RamObject::UUID)
        return m_items->data( m_items->index(section, 0), RamObject::UUID);

    return QAbstractTableModel::headerData(section, orientation, role);
}

RamStatus *RamStatusTableModel::getStatus(RamAbstractItem *item, RamStep *step) const
{
    // Get the statuses for the item
    QSet<RamStatus*> allStatus = getItemStatus(item->uuid());
    // Find the status for the step
    foreach(RamStatus *s, allStatus) if (s->stepUuid() == step->uuid()) return s;
    // Create and return a "NO" status if not found
    return RamStatus::noStatus(item, step);
}

RamStatus *RamStatusTableModel::getStatus(QString itemUuid, QString stepUuid) const
{
    if (itemUuid == "") return nullptr;
    if (stepUuid == "") return nullptr;

    // Get the statuses for the item
    QSet<RamStatus*> allStatus = getItemStatus(itemUuid);

    // Find the status for the step
    foreach(RamStatus *s, allStatus) if (s->stepUuid() == stepUuid) return s;

    // Create and return a "NO" status if not found

    // Get the item
    RamAbstractItem *item;
    if (m_items->type() == RamObject::Asset) item = RamAsset::get(itemUuid);
    else item = RamShot::get(itemUuid);
    if (!item) return nullptr;

    // Get the step
    RamStep *step = RamStep::get(stepUuid);
    if (!step) return nullptr;

    return RamStatus::noStatus(item, step);
}

QSet<RamStatus*> RamStatusTableModel::getItemStatus(QString itemUuid) const
{
    if (itemUuid == "") return QSet<RamStatus*>();
    QSet<RamObject*> allStatus = m_status->lookUp("item", itemUuid);
    QSet<RamStatus*> status;
    foreach(RamObject *statusObj, allStatus)
    {
        RamStatus *s = RamStatus::c(statusObj);
        if (s) status << s;
    }
    return status;
}

QSet<RamStatus *> RamStatusTableModel::getStepStatus(QString stepUuid) const
{
    if (stepUuid == "") return QSet<RamStatus*>();
    QSet<RamObject*> allStatus = m_status->lookUp("step", stepUuid);
    QSet<RamStatus*> status;
    foreach(RamObject *statusObj, allStatus)
    {
        RamStatus *s = RamStatus::c(statusObj);
        if (s) status << s;
    }
    return status;
}

void RamStatusTableModel::stepsInserted(const QModelIndex &parent, int first, int last)
{
    beginInsertColumns(parent, first+1, last+1);

    // Get the steps uuid
    QStringList uuids;
    for (int i = first; i <= last; i++)
    {
        QString uuid = m_steps->data( m_steps->index(i, 0), RamObject::UUID).toString();
        uuids << uuid;
    }
    m_status->addFilterValues("step", uuids);

    endInsertColumns();
}

void RamStatusTableModel::stepsRemoved(const QModelIndex &parent, int first, int last)
{
    beginRemoveColumns(parent, first+1, last+1);
    endRemoveColumns();
}

void RamStatusTableModel::stepsMoved(const QModelIndex &parent, int start, int end, const QModelIndex &destination, int row)
{
    beginMoveColumns(parent, start+1, end+1, destination, row+1);
    endMoveColumns();
}

void RamStatusTableModel::stepsDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles)
{
    Q_UNUSED(roles);

    emit headerDataChanged(Qt::Horizontal, topLeft.row() + 1, bottomRight.row() + 1);
}

void RamStatusTableModel::itemsInserted(const QModelIndex &parent, int first, int last)
{
    beginInsertRows(parent, first, last);
    endInsertRows();
}

void RamStatusTableModel::itemsRemoved(const QModelIndex &parent, int first, int last)
{
    beginRemoveRows(parent, first, last);
    endRemoveRows();
}

void RamStatusTableModel::itemsMoved(const QModelIndex &parent, int start, int end, const QModelIndex &destination, int row)
{
    beginMoveRows(parent, start, end, destination, row);
    endMoveRows();
}

void RamStatusTableModel::itemsDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles)
{
    // The vertical header
    emit headerDataChanged(Qt::Vertical, topLeft.row(), bottomRight.row());

    // The first column has changed too
    emit dataChanged( index( topLeft.row(), 0 ), index( bottomRight.row(), 0), roles);
}

void RamStatusTableModel::statusDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles)
{
    int firstRow = topLeft.row();
    int lastRow = bottomRight.row();

    for (int r = firstRow; r <= lastRow; r++)
    {
        RamObject *statusObj = m_status->get(r);
        if (!statusObj) continue;
        RamStatus *status = RamStatus::c(statusObj);
        int row = m_items->uuidRow( status->itemUuid() );
        if (row < 0) continue;
        int col = m_steps->uuidRow( status->stepUuid() );
        if (col < 0) continue;
        QModelIndex i = index(row, col);
        emit dataChanged( i, i, roles );
    }
}

