#include "ramitem.h"

RamItem::RamItem(QString shortName, QString name, QString uuid, QObject *parent) :
    RamObject(shortName, name, uuid, parent)
{
    _statusHistory = new RamStatusHistory(this);
}

void RamItem::setStatus(RamUser *user, RamState *state, RamStep *step, int completionRatio, QString comment, int version)
{
    RamStatus *status = new RamStatus(user, state, step, this);
    if (completionRatio >= 0) status->setCompletionRatio(completionRatio);
    if (comment != "") status->setComment(comment);
    status->setVersion(version);

    if (this->objectType() == Asset) _dbi->setAssetStatus(_uuid, state->uuid(), step->uuid(), user->uuid(), completionRatio, comment, version, status->uuid());
    else if (this->objectType() == Shot) _dbi->setShotStatus(_uuid, state->uuid(), step->uuid(), user->uuid(), completionRatio, comment, version, status->uuid());

    _statusHistory->append(status);
}

RamStatusHistory *RamItem::statusHistory()
{
    return _statusHistory;
}

QList<RamStatus *> RamItem::statusHistory(RamStep *step)
{
    QList<RamStatus *> history;
    for (int i = 0; i < _statusHistory->count(); i++)
    {
        RamStatus *status = (RamStatus*)_statusHistory->at(i);
        RamStep *s = status->step();
        if (!s) continue;
        if (step->uuid() == s->uuid())
            history << status;
    }

    return history;
}

RamStatus *RamItem::status(QString stepUuid)
{
    for (int i = _statusHistory->count() - 1; i >= 0; i-- )
    {
        RamStatus *status = (RamStatus*)_statusHistory->at(i);
        if (status->step())
            if (status->step()->uuid() == stepUuid) return status;
    }
    return nullptr;
}
