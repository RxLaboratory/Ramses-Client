#ifndef RAMSCHEDULETABLEMODEL_H
#define RAMSCHEDULETABLEMODEL_H

#include <QStringBuilder>
#include "dbtablemodel.h"
#include "ramschedulerow.h"

class RamUser;
class RamScheduleEntry;

/**
 * @brief The RamScheduleTableModel class is the schedule table of a project.
 * It joins together the RamScheduleRow and RamScheduleEntry objects
 * to present the data in a table where the columns are the dates.
 */
class RamScheduleTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    /**
     * @brief RamScheduleTableModel
     * Creates a new empty table.
     * @param parent
     */
    explicit RamScheduleTableModel(QObject *parent = nullptr);
    /**
     * @brief setObjectModel
     * Sets the data to be displayed by the table.
     * @param rows
     * The rows.
     * @param entries
     * The entries.
     */
    void setObjectModel(DBTableModel *rows, DBTableModel *entries);

    // MODEL REIMPLEMENTATION
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    virtual bool moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild) override;

    // Data access helpers
    QVector<RamScheduleEntry *> scheduleEntries(const QModelIndex &index);
    RamScheduleRow *scheduleRow(const QModelIndex &index);

public slots:
    void setStartDate(const QDate &newStartDate);
    void setEndDate(const QDate &newEndDate);

private slots:
    void resetScheduleRows();
    void changeScheduleRows(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles = QVector<int>());
    void insertScheduleRows(const QModelIndex &parent, int first, int last);
    void removeScheduleRows(const QModelIndex &parent, int first, int last);

    void resetEntries();
    void changeEntries(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles = QVector<int>());
    void insertEntries(const QModelIndex &parent, int first, int last);
    void removeEntries(const QModelIndex &parent, int first, int last);

private:
    QModelIndex entryIndex(RamScheduleEntry *e);
    int colForDate(const QDate &date);

    // DATA
    DBTableModel *m_rows = nullptr;
    DBTableModel *m_entries = nullptr;

    // SETTINGS
    QDate m_startDate;
    QDate m_endDate;

    QVariant horizontalHeaderData(int section, int role = Qt::DisplayRole) const;
    QVariant verticalHeaderData(int section, int role = Qt::DisplayRole) const;
};

#endif // RAMSCHEDULETABLEMODEL_H
