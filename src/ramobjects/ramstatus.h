#ifndef RAMSTATUS_H
#define RAMSTATUS_H

#include <QDateTime>

#include "ramuser.h"
#include "ramstate.h"
#include "ramstep.h"

class RamItem;

class RamStatus : public RamObject
{
    Q_OBJECT
public:
    explicit RamStatus(RamUser *user, RamState *state, RamStep *step, RamItem *item);
    explicit RamStatus(RamUser *user, RamState *state, RamStep *step, RamItem *item, QString uuid);
    ~RamStatus();

    int completionRatio() const;
    void setCompletionRatio(int completionRatio);

    RamUser *user() const;

    RamState *state() const;
    void setState(RamState *state);

    QString comment() const;
    void setComment(QString comment);

    int version() const;
    void setVersion(int version);

    RamStep *step() const;

    RamItem *item() const;

    void update();

    QDateTime date() const;
    void setDate(const QDateTime &date);

private:
    int _completionRatio = 50;
    RamUser *_user;
    RamState *_state;
    RamItem *_item;
    QString _comment = "";
    int _version = 1;
    RamStep *_step;
    QDateTime _date;
};

#endif // RAMSTATUS_H
