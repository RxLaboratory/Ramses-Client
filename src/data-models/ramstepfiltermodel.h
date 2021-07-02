#ifndef RAMSTEPTYPEFILTERMODEL_H
#define RAMSTEPTYPEFILTERMODEL_H

#include <QSortFilterProxyModel>

#include "ramstep.h"

/**
 * @brief The RamStepFilterModel class is used to filter a RamObjectList of steps according to their type (asset, shot, pre, post production)
 * It is also able to ignore specific steps (using their uuid); this is used to filter steps with a checkable list in Ramses' tables.
 * It must NOT be used with anything else than a RamObjectList of RamStep!
 * @todo The uuid filtering should be moved to RamObjectFilterModel, and RamStepFilterModel may inherit RamObjectFilterModel,
 * which should avoid the need of multiple filter models chained one after the other.
 */
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
    RamObjectList *m_emptyList;
};

#endif // RAMSTEPTYPEFILTERMODEL_H
