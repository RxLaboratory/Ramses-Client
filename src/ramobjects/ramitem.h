#ifndef RAMITEM_H
#define RAMITEM_H

#include "ramstatus.h"
#include "ramstepstatushistory.h"
#include "ramobjectuberlist.h"

class RamProject;

/**
 * @brief The RamItem class is the base class for RamShot and RamAsset
 * It handles the status history of the Shot or the Item
 */
class RamItem : public RamObject
{
    Q_OBJECT
public:
    explicit RamItem(QString shortName, RamProject *project, QString name = "", QString uuid = "", QObject *parent = nullptr);

    void setStatus(RamUser *user, RamState *state, RamStep *step, int completionRatio = -1, QString comment = "", int version = 1);
    void addStatus(RamStatus *status);
    RamObjectUberList *statusHistory() const;
    RamStepStatusHistory *statusHistory(RamStep *step) const;
    RamStepStatusHistory *statusHistory(QString stepUuid) const;
    RamStatus *status(QString stepUuid) const;

    RamProject *project() const;

    RamStep::Type productionType() const;

protected:
    void setProductionType(RamStep::Type newProductionType);

private slots:
    void newStep(RamObject *stepObj);
    void stepRemoved(RamObject *stepObj);

private:
    RamObjectUberList *_statusHistory;
    RamProject *m_project;
    RamStep::Type m_productionType;
};

#endif // RAMITEM_H
