#ifndef RAMITEM_H
#define RAMITEM_H

#include "ramstatus.h"
#include "ramstepstatushistory.h"
#include "ramobjectuberlist.h"

/**
 * @brief The RamItem class is the base class for RamShot and RamAsset
 * It handles the status history of the Shot or the Item
 */
class RamItem : public RamObject
{
    Q_OBJECT
public:
    explicit RamItem(QString shortName, QString name = "", QString uuid = "", QObject *parent = nullptr);

    void setStatus(RamUser *user, RamState *state, RamStep *step, int completionRatio = -1, QString comment = "", int version = 1);
    void addStatus(RamStatus *status);
    RamObjectUberList *statusHistory() const;
    RamStepStatusHistory *statusHistory(RamStep *step) const;
    RamStepStatusHistory *statusHistory(QString stepUuid) const;
    RamStatus *status(QString stepUuid) const;

private:
    RamObjectUberList *_statusHistory;
};

#endif // RAMITEM_H
