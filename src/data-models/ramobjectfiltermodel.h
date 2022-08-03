#ifndef RAMOBJECTFILTERMODEL_H
#define RAMOBJECTFILTERMODEL_H

#include <QSortFilterProxyModel>

#include "ramobject.h"
#include "ramobjectlist.h"

/**
 * @brief The RamObjectFilterModel class is a proxy used to filter and search items displayed in Ramses' lists.
 * It filters RamObjects according to ther filterUuid property, or if their shortName/name corresponds to a search string.
 */
class RamObjectFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit RamObjectFilterModel(QObject *parent = nullptr);
    void setList(QAbstractItemModel *list);
    void setFilterUuid(const QString &filterUuid);
    void search(const QString &searchStr);

signals:
    void aboutToFilter();

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
    /**
     * @brief prepareFilter calls invalidateFilter but emits aboutToFilter() first.
     */
    void prepareFilter();

private:
    QString m_currentFilterUuid;
    QString m_searchString;
    RamObjectList<RamObject *> *m_emptyList;
};

#endif // RAMOBJECTFILTERMODEL_H
