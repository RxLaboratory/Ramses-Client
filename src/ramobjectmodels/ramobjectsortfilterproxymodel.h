#ifndef RAMOBJECTSORTFILTERPROXYMODEL_H
#define RAMOBJECTSORTFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>

#include "ramobject.h"

/**
 * @brief The RamObjectSortFilterProxyModel class is a proxy used to filter and sort RamObjectModel.
 * setFilterList: this will be used as a filter list, adds an "All item" on top
 * Filters: by RamObject.filterUuid, name / shortname
 */
class RamObjectSortFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    // Construct and prepare
    explicit RamObjectSortFilterProxyModel(QObject *parent = nullptr);
    explicit RamObjectSortFilterProxyModel(QString listName, QObject *parent = nullptr);
    void setFilterList();
    void setSingleColumn(bool singleColumn = true);

    // Model reimplementation
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QModelIndex mapFromSource(const QModelIndex &sourceIndex) const override;
    QModelIndex mapToSource(const QModelIndex &proxyIndex) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &child) const override;

    // Convenience access
    // An object by its row
    RamObject *get(int row) const;
    // An object by index
    RamObject *get(QModelIndex index) const;
    // An object by its shortname, or name
    RamObject *searchObject(QString searchString) const;

    RamObject::ObjectType type() const;

    // Set filters
    void setFilterUuid(const QString &filterUuid);
    void search(const QString &searchStr);

signals:
    void aboutToFilter();

protected:
    virtual bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;

private:
    void prepareFilter();

    // Config
    QString m_listName;
    bool m_isFilterList;
    bool m_isSingleColumn = false;

    // Filters
    QString m_searchString;
    QString m_currentFilterUuid;
};

#endif // RAMOBJECTSORTFILTERPROXYMODEL_H
