#ifndef RAMITEM_H
#define RAMITEM_H

#include "ramstatus.h"
#include "data-models/ramobjectlist.h"
#include "data-models/ramstepstatushistory.h"

class RamProject;

/**
 * @brief The RamItem class is the base class for RamShot and RamAsset
 * It handles the status history of the Shot or the Item
 */
class RamItem : public RamObject
{
    Q_OBJECT
public:
    explicit RamItem(QString shortName, RamProject *project, QString name = "", QString uuid = "");

    RamStatus *setStatus(RamUser *user, RamState *state, RamStep *step, int completionRatio = -1, QString comment = "", int version = 1);
    void addStatus(RamStatus *status);

    QMap<QString, RamStepStatusHistory *> statusHistory() const;
    RamStepStatusHistory *statusHistory(RamObject *stepObj);
    RamStatus *status(RamObject *step);
    QList<RamStatus*> status();

    RamUser *assignedUser(RamStep *step);

    RamProject *project() const;
    RamStep::Type productionType() const;
    static RamItem *item(QString uuid);

    QString previewImagePath() const;

signals:
    void statusChanged(RamItem *, RamStep *);

protected:
    void setProductionType(RamStep::Type newProductionType);

private slots:
    // Relay status changed if needed (if the last one has changed only)
    void insertStatus(const QModelIndex &parent,int first ,int last);
    void statusChanged(const QModelIndex &first, const QModelIndex &last);
    void removeStatus(const QModelIndex &parent,int first ,int last);
    void statusMoved(const QModelIndex &parent, int start, int end, const QModelIndex &destination, int row);
    void statusCleared();

private:
    QMap<QString, RamStepStatusHistory*> m_history;
    RamProject *m_project;
    RamObjectList *m_steps;
    RamStep::Type m_productionType;

    QMap<QString, QMetaObject::Connection> m_stepConnections;
    QMap<QString, QList<QMetaObject::Connection>> m_statusConnections;
};

#endif // RAMITEM_H
