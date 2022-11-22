#ifndef RAMSTATUSTABLEMODEL_H
#define RAMSTATUSTABLEMODEL_H

#include "dbtablefilterproxymodel.h"
#include "ramstatus.h"

class RamStatusTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit RamStatusTableModel(DBTableFilterProxyModel *steps, DBTableFilterProxyModel *items, QObject *parent = nullptr);

    void load();

    // === QAbstractTableModel Reimplementation ===

    // Data access
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Status Access

    // With the actual item and step, slightly better perf than with Uuids if they already exist
    RamStatus *getStatus(RamAbstractItem *item, RamStep *step) const;
    RamStatus *getStatus(QString itemUuid, QString stepUuid) const;
    QSet<RamStatus*> getItemStatus(QString itemUuid) const;
    QSet<RamStatus*> getStepStatus(QString stepUuid) const;

private slots:
    // We need to insert/remove rows and columns
    // and monitor changes

    void stepsInserted(const QModelIndex &parent, int first, int last);
    void stepsRemoved(const QModelIndex &parent, int first, int last);
    void stepsMoved(const QModelIndex &parent, int start, int end, const QModelIndex &destination, int row);
    void stepsDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles = QVector<int>());

    void itemsInserted(const QModelIndex &parent, int first, int last);
    void itemsRemoved(const QModelIndex &parent, int first, int last);
    void itemsMoved(const QModelIndex &parent, int start, int end, const QModelIndex &destination, int row);
    void itemsDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles = QVector<int>());

    void statusDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles = QVector<int>());

private:
    DBTableModel *m_status;
    DBTableFilterProxyModel *m_steps;
    DBTableFilterProxyModel *m_items;

    bool m_loaded = false;
};

#endif // RAMSTATUSTABLEMODEL_H
