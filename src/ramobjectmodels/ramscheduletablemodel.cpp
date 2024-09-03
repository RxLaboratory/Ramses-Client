#include "ramscheduletablemodel.h"

#include "duapp/app-config.h"
#include "duapp/dusettings.h"
#include "duutils/stringutils.h"
#include "ramscheduleentry.h"
#include "ramschedulerow.h"
#include "ramses.h"
#include "ramstep.h"

RamScheduleTableModel::RamScheduleTableModel(QObject *parent) : QAbstractTableModel(parent)
{
    m_startDate = QDate::currentDate().addDays(-5);
    m_endDate = QDate::currentDate();
}

void RamScheduleTableModel::setObjectModel(DBTableModel *rows, DBTableModel *entries)
{
    beginResetModel();

    if (m_rows) disconnect(m_rows, nullptr, this, nullptr);
    if (m_entries) disconnect(m_entries, nullptr, this, nullptr);

    m_rows = rows;
    m_entries = entries;

    if (m_rows)
    {
        connect( m_rows, &DBTableModel::rowsInserted, this, &RamScheduleTableModel::insertScheduleRows);
        connect( m_rows, &DBTableModel::rowsAboutToBeRemoved, this, &RamScheduleTableModel::removeScheduleRows);
        connect( m_rows, &DBTableModel::dataChanged, this, &RamScheduleTableModel::changeScheduleRows);
        connect( m_rows, &DBTableModel::modelReset, this, &RamScheduleTableModel::resetScheduleRows);
    }

    if (m_entries)
    {
        connect( m_entries, &DBTableModel::rowsInserted, this,&RamScheduleTableModel::insertEntries);
        connect( m_entries, &DBTableModel::rowsAboutToBeRemoved, this, &RamScheduleTableModel::removeEntries);
        connect( m_entries, &DBTableModel::dataChanged, this, &RamScheduleTableModel::changeEntries);
        connect( m_entries, &DBTableModel::modelReset, this, &RamScheduleTableModel::resetEntries);
    }

    endResetModel();
}

int RamScheduleTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    if (!m_entries) return 0;

    return m_startDate.daysTo( m_endDate ) + 1;
}

int RamScheduleTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    if (m_rows)
        return m_rows->rowCount();

    return 0;
}

QVariant RamScheduleTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Vertical)
        return verticalHeaderData(section, role);

    return horizontalHeaderData(section, role);
}

QVariant RamScheduleTableModel::data(const QModelIndex &index, int role) const
{
    if (!m_rows) return QVariant();
    if (!m_entries) return QVariant();

    int row = index.row();
    int col = index.column();

    QDate date = horizontalHeaderData(col, RamObject::Date).toDate();

    if (role ==  RamObject::Date )
        return date;

    // Get the entrie(s)
    QSet<RamObject*> entriesSet = m_entries->lookUp("date", date.toString( DATE_DATA_FORMAT ));
    m_entries->intersectLookUp("row",  m_rows->getUuid(row), entriesSet);

    // Empty cell
    if (entriesSet.isEmpty())
        return QVariant();

    // The type
    if (role == RamAbstractObject::Type)
        return RamObject::ScheduleEntry;

    // Make sure the steps are always returned in the same order
    QList<RamObject*> entries = entriesSet.values();
    std::sort( entries.begin(), entries.end(), [] (RamObject *a, RamObject *b) {
        auto entryA = RamScheduleEntry::c(a);
        auto entryB = RamScheduleEntry::c(b);

        // Don't actually get the RamStep
        // To improve performance just use the uuid
        return entryA->getData("step").toString() < entryB->getData("step").toString();

        // ======= Disabled better sorting
        auto stepA = entryA->step();
        auto stepB = entryB->step();

        if (stepA && stepB)
            return stepA->order() < stepB->order();

        if (!stepA && stepB)
            return false;
        if (stepA && !stepB)
            return true;

        return entryA->shortName() < entryB->shortName();
        // ==========
    });


    // Pointer list for Multiple entries,
    // The delegate should get the list of pointers
    // to do something with it...
    if (role == RamAbstractObject::Pointer) {
        QVariantList ptrs;
        for (auto entry: qAsConst(entries)) {
            ptrs << reinterpret_cast<qintptr>(entry);
        }
        return ptrs;
    }

    // QStringList for UUIDs
    if (role == RamAbstractObject::UUID) {
        QStringList uuids;
        for (auto entry: qAsConst(entries)) {
            uuids << entry->uuid();
        }
        return uuids;
    }

    // Single entry, return the data
    if (entries.count() == 1) {
        return entries.first()->roleData(role);
    }


    //*/
    return QVariant();

}

bool RamScheduleTableModel::moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild)
{
    return m_rows->moveRows(sourceParent, sourceRow, count, destinationParent, destinationChild);
}

QVector<RamScheduleEntry *> RamScheduleTableModel::scheduleEntries(const QModelIndex &index)
{
    QVector<RamScheduleEntry*> entries;

    const QVariantList list = data(index, RamAbstractObject::Pointer).toList();

    for (const auto &i: list) {
        qintptr iptr = i.toULongLong();
        if (iptr)
            entries << reinterpret_cast<RamScheduleEntry*>(iptr);
    }

    return entries;
}

RamScheduleRow *RamScheduleTableModel::scheduleRow(const QModelIndex &index)
{
    int r = index.row();
    if (r < 0 || r >= m_rows->count())
        return nullptr;

    return RamScheduleRow::c(
        m_rows->get(r)
        );
}

void RamScheduleTableModel::resetEntries()
{
    beginResetModel();
    endResetModel();
}

void RamScheduleTableModel::changeEntries(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles)
{
    if (!m_entries) return;
    int first = topLeft.row();
    int last = bottomRight.row();
    for (int i = first; i <= last; i++)
    {
        RamScheduleEntry *e = RamScheduleEntry::c( m_entries->get(i) );
        QModelIndex ind = entryIndex(e);
        emit dataChanged(ind, ind, roles);
    }
}

void RamScheduleTableModel::insertEntries(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent);

    if (!m_entries) return;
    for (int i = first; i <= last; i++)
    {
        RamScheduleEntry *e = RamScheduleEntry::c( m_entries->get(i) );
        QModelIndex ind = entryIndex(e);
        emit dataChanged(ind, ind, QVector<int>());
    }
}

void RamScheduleTableModel::removeEntries(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent);

    if (!m_entries) return;
    for (int i = first; i <= last; i++)
    {
        RamScheduleEntry *e = RamScheduleEntry::c( m_entries->get(i) );
        QModelIndex ind = entryIndex(e);
        emit dataChanged(ind, ind, QVector<int>());
    }
}

QModelIndex RamScheduleTableModel::entryIndex(RamScheduleEntry *e)
{
    if (!e) return QModelIndex();

    // Get the row
    RamScheduleRow *row = e->row();
    int r = m_rows->objectRow(row);

    // Get the column
    QDate date = e->date();
    int c = colForDate(date);

    return this->index(r, c);
}

int RamScheduleTableModel::colForDate(const QDate &date)
{
    return m_startDate.daysTo(date);
}

QVariant RamScheduleTableModel::horizontalHeaderData(int section, int role) const
{
    QDate date = m_startDate.addDays(section);

    if ( role == Qt::DisplayRole )
        return StringUtils::capitalize( date.toString("MMMM\n") ) + // April
               StringUtils::capitalize( date.toString("dddd\n") ) + // Monday
               date.toString("yyyy-MM-dd"); // TODO Use settings, 2024-04-22

    if (role == Qt::ForegroundRole)
    {
        if (date == QDate::currentDate())
            return QBrush(DuSettings::i()->get(DuSettings::UI_FocusColor).value<QColor>());

        RamProject *proj = Ramses::instance()->currentProject();
        if (proj && date == proj->deadline())
            return QBrush(QColor(249,105,105));

        if (date.weekNumber() % 2 == 1)
            return QBrush(QColor(130,130,130));
        else
            return QBrush(QColor(170,170,170));
    }

    if (role ==  RamObject::Date)
        return date;

    return QVariant();
}

QVariant RamScheduleTableModel::verticalHeaderData(int section, int role) const
{
    // Invalid index
    if (section < 0 || section > m_rows->rowCount())
        return QVariant();

    // General
    if (role == Qt::TextAlignmentRole) return Qt::AlignCenter;

    return m_rows->data(
        m_rows->index(section, 0),
        role
        );
}

void RamScheduleTableModel::setEndDate(const QDate &newEndDate)
{
    if (newEndDate < m_startDate) return;

    if (newEndDate > m_endDate)
    {
        beginInsertColumns(QModelIndex(), columnCount(), columnCount() + m_endDate.daysTo(newEndDate) -1);
        m_endDate = newEndDate;
        endInsertColumns();
    }
    else if (newEndDate < m_endDate)
    {
        beginRemoveColumns(QModelIndex(), columnCount() - newEndDate.daysTo(m_endDate) , columnCount() -1);
        m_endDate = newEndDate;
        endRemoveColumns();
    }
}

void RamScheduleTableModel::resetScheduleRows()
{
    beginResetModel();
    endResetModel();
}

void RamScheduleTableModel::changeScheduleRows(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles)
{
    if (!m_rows)
        return;

    int first = topLeft.row();
    int last = bottomRight.row();


    QModelIndex tl = index(first, 0);
    QModelIndex br = index(last, columnCount());

    emit headerDataChanged(Qt::Vertical, first, last);
    emit dataChanged(tl, br, roles);
}

void RamScheduleTableModel::insertScheduleRows(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent);

    if (!m_rows)
        return;

    //We're inserting new rows
    beginInsertRows(QModelIndex(), first, last);
    // Finished!
    endInsertRows();
}

void RamScheduleTableModel::removeScheduleRows(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent);

    if (!m_rows)
        return;

    // We're removing rows
    beginRemoveRows(QModelIndex(), first, last);
    endRemoveRows();
}

void RamScheduleTableModel::setStartDate(const QDate &newStartDate)
{
    if (newStartDate > m_endDate) return;

    if (newStartDate < m_startDate)
    {
        beginInsertColumns(QModelIndex(), 0, newStartDate.daysTo(m_startDate)-1);
        m_startDate = newStartDate;
        endInsertColumns();
    }
    else if (newStartDate > m_startDate)
    {
        beginRemoveColumns(QModelIndex(), 0, m_startDate.daysTo(newStartDate) -1);
        m_startDate = newStartDate;
        endRemoveColumns();
    }
}

