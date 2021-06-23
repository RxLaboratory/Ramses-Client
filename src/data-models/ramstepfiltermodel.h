#ifndef RAMSTEPTYPEFILTERMODEL_H
#define RAMSTEPTYPEFILTERMODEL_H

#include <QSortFilterProxyModel>

#include "ramstep.h"

class RamStepFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit RamStepFilterModel(RamStep::Type stepType, QObject *parent = nullptr);
    void setList(RamObjectList *list);
    void ignoreUuid(QString uuid);
    void acceptUuid(QString uuid);

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;

private:
    RamStep::Type m_stepType;
    QStringList m_ignoreUuids;
    RamObjectList *m_objectList = nullptr;
};

#endif // RAMSTEPTYPEFILTERMODEL_H
