#ifndef RAMOBJECTFILTERLIST_H
#define RAMOBJECTFILTERLIST_H

#include <QSortFilterProxyModel>

#include "ramobjectlist.h"

class RamObjectFilterList : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    RamObjectFilterList(QObject *parent = nullptr);
    void setList(RamObjectList *list);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QModelIndex mapFromSource(const QModelIndex &sourceIndex) const override;
    QModelIndex mapToSource(const QModelIndex &proxyIndex) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &child) const override;

private:
    RamObjectList *m_objectList = nullptr;
};

#endif // RAMOBJECTFILTERLIST_H
