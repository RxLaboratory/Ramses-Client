#include "ramstatustablemodel.h"

#include "duapp/app-config.h"
#include "ramstatus.h"
#include "ramasset.h"
#include "ramshot.h"
#include "ramstate.h"
#include "ramuser.h"
#include "statemanager.h"

RamStatusTableModel::RamStatusTableModel(DBTableModel *steps, DBTableModel *items, QObject *parent)
    : QAbstractTableModel{parent}
{
    m_steps = steps;
    m_items = items;

    m_status = new DBTableModel(RamObject::Status, true, false, this);
    m_status->addFilterValues("step", m_steps->toStringList());
    m_status->addLookUpKey("item");
    m_status->addLookUpKey("step");
    m_status->addLookUpKey("dueDate");

    // Update cache only when idle or going back to idle
    connect(StateManager::i(), &StateManager::stateChanged,
            this, [this] (StateManager::State state) {
        if (state == StateManager::Idle)
            this->suspendEstimations(false);
        else
            this->suspendEstimations(true);
    });
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

    connect(m_items, &DBTableModel::rowsInserted, this, &RamStatusTableModel::cacheEstimations);
    connect(m_items, &DBTableModel::rowsRemoved, this, &RamStatusTableModel::cacheEstimations);
    connect(m_items, &DBTableModel::dataChanged, this, &RamStatusTableModel::cacheEstimations);

    connect(m_status, &DBTableModel::dataChanged, this, &RamStatusTableModel::statusDataChanged);

    cacheEstimations();
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

        QString stepUuid = m_steps->getUuid( section-1 );

        if (role == RamObject::Estimation) return stepEstimation(stepUuid);
        if (role == RamObject::Completion) return stepCompletionRatio(stepUuid);
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

RamStatus *RamStatusTableModel::getStatus(RamObject *itemObj, RamStep *step) const
{
    // Get the statuses for the item
    QSet<RamStatus*> allStatus = getItemStatus(itemObj->uuid());
    // Find the status for the step
    foreach(RamStatus *s, allStatus) if (s->stepUuid() == step->uuid()) return s;
    RamAbstractItem *item = reinterpret_cast<RamAbstractItem*>(itemObj);
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

QSet<RamStatus *> RamStatusTableModel::getStatus(const QDate &date, const QString userUuid) const
{
    if (!date.isValid()) return QSet<RamStatus *>();

    const QSet<RamObject *> all = m_status->lookUp("dueDate", date.toString(DATE_DATA_FORMAT));
    QSet<RamStatus *> allStatus;
    for (auto status: all) {
        auto st = qobject_cast<RamStatus*>(status);
        if (!st) continue;

        if (userUuid == "")
            allStatus << st;

        if (!st->assignedUser())
            continue;

        if (userUuid == st->assignedUser()->uuid())
            allStatus << st;
    }
    return allStatus;
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

void RamStatusTableModel::suspendEstimations(bool s)
{
    if (m_cacheSuspended == s)
        return;

    m_cacheSuspended = s;
    if (!s)
        cacheEstimations();
}

float RamStatusTableModel::stepEstimation(const QString &stepUuid, const QString &userUuid) const
{
    if (userUuid == "")
        return m_estimations.value(stepUuid).estimation;

    return m_userestimations.value(
        QPair<QString,QString>(stepUuid, userUuid)
        ).estimation;
}

int RamStatusTableModel::stepCompletionRatio(const QString &stepUuid, const QString &userUuid) const
{
    if (userUuid == "")
        return m_estimations.value(stepUuid).completionRatio;

    return m_userestimations.value(
        QPair<QString,QString>(stepUuid, userUuid)
        ).completionRatio;
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
    // Remove unneeded cache
    for (int i = first; i <= last; i++) {
        m_estimations.remove( m_steps->getUuid(i) );

    }
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

    // The default estimation may have changed, we need to recompute estimations
    int first = topLeft.row();
    int last = bottomRight.row();
    for (int i = first; i <= last; i++) {
        cacheStepEstimation( m_steps->getUuid(i) );
    }

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
        QString stepUuid =  status->stepUuid();
        int col = m_steps->uuidRow( stepUuid );
        if (col < 0) continue;

        cacheStepEstimation( stepUuid );

        QModelIndex i = index(row, col);
        emit dataChanged( i, i, roles );
    }
}

void RamStatusTableModel::cacheStepEstimation(QString stepUuid)
{
    m_cacheIsOutdated = true;
    if (m_cacheSuspended)
        return;
    m_cacheIsOutdated = false;

    qDebug() << "Caching step estimation" << stepUuid;

    if (stepUuid == "") return;

    // Clear this step estimations
    m_estimations.remove(stepUuid);
    QHashIterator<QPair<QString,QString>, StepEstimation> i(m_userestimations);
    while (i.hasNext()) {
        i.next();
        if (i.key().first != stepUuid)
            continue;
        m_userestimations.remove(i.key());
    }

    // For each item, get the status
    for (int i = 0; i < m_items->rowCount(); i ++) {
        RamStatus *status = getStatus( m_items->getUuid(i), stepUuid);
        if (!status) continue;

        updateStepEstimCache(status, stepUuid);
    }

    // Update completion ratios

    QMutableHashIterator<QPair<QString,QString>, StepEstimation> ius(m_userestimations);
    while (ius.hasNext()) {
        ius.next();
        if (ius.key().first != stepUuid)
            continue;
        ius.value().updateCompletionRatio();
    }

    StepEstimation stepEstim = m_estimations.value(stepUuid);
    stepEstim.updateCompletionRatio();
    m_estimations.insert(stepUuid, stepEstim);

    qDebug() << "Step estimation cached" << stepUuid;

    emit stepEstimationChanged( stepUuid );
    emit estimationsChanged();
}

void RamStatusTableModel::cacheEstimations()
{
    m_cacheIsOutdated = true;
    if (m_cacheSuspended)
        return;

    m_estimations.clear();
    m_userestimations.clear();

    for (int i = 0; i < m_items->rowCount(); i++) {
        QSet<RamStatus*> allStatus = getItemStatus( m_items->getUuid(i) );
        foreach(RamStatus *status, allStatus) {

            QString stepUuid = status->stepUuid();
            if (stepUuid == "") continue;

            updateStepEstimCache(status, stepUuid);
        }
    }

    // Update completion ratios

    QHash<QPair<QString,QString>, StepEstimation>::iterator ius = m_userestimations.begin();
    while (ius != m_userestimations.end()) {
        ius.value().updateCompletionRatio();
        ius++;
    }

    QHash<QString, StepEstimation>::iterator is = m_estimations.begin();
    while (is != m_estimations.end()) {
        is.value().updateCompletionRatio();
        is++;
    }

    m_cacheIsOutdated = false;

    emit estimationsChanged();
}

void RamStatusTableModel::updateStepEstimCache(RamStatus *status, const QString &stepUuid)
{
    RamState *state = status->state();
    if (!state)
        return;
    if (state->shortName() == "NO")
        return;

    float estimation = 0;
    if (status->useAutoEstimation()) estimation = status->estimation();
    else estimation = status->goal();

    float completionRatio = status->completionRatio();

    StepEstimation stepEstim = m_estimations.value(stepUuid);
    stepEstim.estimation += estimation;
    stepEstim.total++;
    stepEstim.completionRatio += completionRatio;
    m_estimations.insert( stepUuid, stepEstim );

    RamUser *user = status->assignedUser();
    if (user) {
        QPair<QString,QString> k(stepUuid, user->uuid());

        StepEstimation stepUserEstim = m_userestimations.value( k );
        stepUserEstim.estimation += estimation;
        stepUserEstim.total++;
        stepUserEstim.completionRatio += completionRatio;
        m_userestimations.insert( k, stepUserEstim );
    }
}
