#ifndef RAMOBJECTFILTERMODEL_H
#define RAMOBJECTFILTERMODEL_H

#include <QSortFilterProxyModel>

#include "ramobject.h"
#include "ramobjectlist.h"

class RamObjectFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit RamObjectFilterModel(QObject *parent = nullptr);
    void setList(QAbstractItemModel *list);
    void setFilterUuid(const QString &filterUuid);
    void search(const QString &searchStr);

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;

private:
    QString m_currentFilterUuid;
    QString m_searchString;
    RamObjectList *m_emptyList;
};

#endif // RAMOBJECTFILTERMODEL_H
