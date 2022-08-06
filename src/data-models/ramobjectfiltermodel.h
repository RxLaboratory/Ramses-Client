#ifndef RAMOBJECTFILTERMODEL_H
#define RAMOBJECTFILTERMODEL_H

#include <QSortFilterProxyModel>

#include "data-models/ramitemtable.h"

template<typename RO> class RamObjectList;

/**
 * @brief The RamObjectFilterModel class is a proxy used to filter and search items displayed in Ramses' lists.
 * It filters RamObjects according to ther filterUuid property, or if their shortName/name corresponds to a search string.
 */
template<typename RO> class RamObjectFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit RamObjectFilterModel(QObject *parent = nullptr);
    void setList(RamObjectList<RO> *list);
    void setList(RamItemTable *list);
    void setFilterUuid(const QString &filterUuid);
    void search(const QString &searchStr);

    // LIST INFORMATION
    RO at(int i) const;
    RO at(QModelIndex i) const;

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
    RamObjectList<RO> *m_emptyList;
};

#endif // RAMOBJECTFILTERMODEL_H
