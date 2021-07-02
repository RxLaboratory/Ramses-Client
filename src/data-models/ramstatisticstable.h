#ifndef RAMSTATISTICSTABLE_H
#define RAMSTATISTICSTABLE_H

#include <QAbstractTableModel>
#include <QStringBuilder>

#include "ramses.h"

class RamStatisticsTable : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit RamStatisticsTable(QObject *parent = nullptr);

    // MODEL REIMPLEMENTATION
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private slots:
    void changeProject(RamProject *project);

    void insertStep(const QModelIndex &parent, int first, int last);
    void removeStep(const QModelIndex &parent, int first, int last);

    void estimationComputed();

private:

    RamProject *m_project = nullptr;

    // Connect submodels and relay events
    void connectEvents();
};

#endif // RAMSTATISTICSTABLE_H
