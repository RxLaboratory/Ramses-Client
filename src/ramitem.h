#ifndef RAMITEM_H
#define RAMITEM_H

#include "ramstatus.h"

class RamItem : public RamObject
{
    Q_OBJECT
public:
    explicit RamItem(QString shortName, QString name = "", QString uuid = "", QObject *parent = nullptr);

    void setStatus(RamUser *user, RamState *state, RamStep *step, int completionRatio = -1, QString comment = "", int version = 1);
    QList<RamStatus*> statusHistory();
    QList<RamStatus*> statusHistory(RamStep *step);
    void addStatus(RamStatus *status);
    void removeStatus(RamStatus *status);
    void removeStatus(QString uuid);
    void sortStatusHistory();

signals:
    void newStatus(RamStatus *);
    void statusRemoved(RamStatus *);

private:
    QList<RamStatus*> _statusHistory;
};

#endif // RAMITEM_H
