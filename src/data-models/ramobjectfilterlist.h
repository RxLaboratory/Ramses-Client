#ifndef RAMOBJECTFILTERLIST_H
#define RAMOBJECTFILTERLIST_H

#include <QSortFilterProxyModel>

#include "ramobjectlist.h"

/**
 * @brief The RamObjectFilterList class is used to add an "ALL" item in the top of a list,
 * Used by QComboBox when displaying a list of filters.
 */
template<typename RO> class RamObjectFilterList : public QSortFilterProxyModel
{
public:
    RamObjectFilterList(QObject *parent = nullptr);
    void setList(RamObjectList<RO> *list);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QModelIndex mapFromSource(const QModelIndex &sourceIndex) const override;
    QModelIndex mapToSource(const QModelIndex &proxyIndex) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &child) const override;

    // LIST INFO
    RO at(int i );

private:
    RamObjectList<RO> *m_objectList = nullptr;
};

#endif // RAMOBJECTFILTERLIST_H
