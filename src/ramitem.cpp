#include "ramitem.h"

RamItem::RamItem(QString shortName, QString name, QString uuid, QObject *parent) :
    RamObject(shortName, name, uuid, parent)
{

}

void RamItem::setStatus(RamUser *user, RamState *state, RamStep *step, int completionRatio, QString comment, int version)
{
    RamStatus *status = new RamStatus(user, state, step, this);
    if (completionRatio >= 0) status->setCompletionRatio(completionRatio);
    if (comment != "") status->setComment(comment);
    status->setVersion(version);

    if (this->objectType() == Asset) _dbi->setAssetStatus(_uuid, state->uuid(), step->uuid(), user->uuid(), completionRatio, comment, version, status->uuid());
    else if (this->objectType() == Shot) _dbi->setShotStatus(_uuid, state->uuid(), step->uuid(), user->uuid(), completionRatio, comment, version, status->uuid());

    addStatus(status);
}

QList<RamStatus *> RamItem::statusHistory()
{
    return _statusHistory;
}

QList<RamStatus *> RamItem::statusHistory(RamStep *step)
{
    QList<RamStatus *> history;
    foreach(RamStatus *status, _statusHistory)
        if (status->step()->uuid() == step->uuid())
            history << status;

    return history;
}

void RamItem::addStatus(RamStatus *status)
{
    connect(this, &RamObject::removed, status, &RamObject::remove);
    _statusHistory << status;
    emit newStatus(status);
}

void RamItem::removeStatus(RamStatus *status)
{
    removeStatus(status->uuid());
}

void RamItem::removeStatus(QString uuid)
{
    for (int i = _statusHistory.count() -1; i >= 0; i--)
    {
        RamStatus *s = _statusHistory[i];
        if (s->uuid() == uuid)
        {
            s = _statusHistory.takeAt(i);
            emit statusRemoved(s);
        }
    }
}

bool statusSorter(RamStatus *s1, RamStatus *s2)
{
    return s1->date() < s2->date();
}

void RamItem::sortStatusHistory()
{
    std::sort( _statusHistory.begin(), _statusHistory.end(), statusSorter);
}
