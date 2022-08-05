#ifndef RAMITEM_H
#define RAMITEM_H

#include "ramstep.h"

class RamStepStatusHistory;
class RamStatus;
template<typename RO> class RamObjectList;

class RamState;

/**
 * @brief The RamItem class is the base class for RamShot and RamAsset
 * It handles the status history of the Shot or the Item
 */
class RamItem : public RamObject
{
    Q_OBJECT
public:

    // STATIC //

    static RamItem *getObject(QString uuid, bool constructNew = false);

    // OTHER //

    RamItem(QString shortName, QString name, RamStep::Type productionType, RamProject *project );

    RamProject *project() const;
    RamStep::Type productionType() const;

    QMap<QString, RamStepStatusHistory *> statusHistory() const;
    RamStepStatusHistory *statusHistory(RamObject *stepObj);

    RamStatus *setStatus(RamUser *user, RamState *state, RamStep *step, int completionRatio = -1, QString comment = "", int version = 1);
    void addStatus(RamStatus *status);

    /**
     * @brief status The latest (current) status for a specific step
     * @param step
     * @return The status
     */
    RamStatus *status(RamStep *step);
    /**
     * @brief status All the latest (current) status
     * @return The latest status for each step
     */
    QList<RamStatus*> status();

    RamUser *assignedUser(RamStep *step);
    bool isUserAssigned(RamUser *u, RamStep *step = nullptr);
    bool isUnassigned(RamStep *step = nullptr);

    bool hasState(RamState *state, RamStep *step = nullptr);

    QString previewImagePath() const;

signals:
    void statusChanged(RamItem *, RamStep *);

protected:
    RamItem(QString uuid);
    // Immutable data
    // item can't be transfered (yet)
    // and shots/assets can't be converted to one another
    RamProject *m_project;
    RamStep::Type m_productionType;

private slots:
    void latestStatusChanged(RamStepStatusHistory*history);

private:
    void construct();
    // Monitors changes to emit statusChanged
    void connectHistory(RamStepStatusHistory *history);

    QMap<QString, RamStepStatusHistory*> m_history;
};

#endif // RAMITEM_H
