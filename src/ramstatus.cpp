#include "ramstatus.h"

RamStatus::RamStatus(RamUser *user, RamState *state, RamStep *step, QObject *parent):
    RamObject("", "", "", parent)
{
    _user = user;
    _state = state;
    _step = step;
    _completionRatio = _state->completionRatio();
    _date = QDateTime::currentDateTimeUtc();

    setObjectType(Status);
}

RamStatus::RamStatus(RamUser *user, RamState *state, RamStep *step, QString uuid, QObject *parent):
    RamObject("", "", uuid, parent)
{
    _user = user;
    _state = state;
    _step = step;
    _completionRatio = _state->completionRatio();
    _date = QDateTime::currentDateTimeUtc();

    setObjectType(Status);
}

int RamStatus::completionRatio() const
{
    return _completionRatio;
}

void RamStatus::setCompletionRatio(int completionRatio)
{
    _completionRatio = completionRatio;
    emit changed(this);
}

RamUser *RamStatus::user() const
{
    return _user;
}

void RamStatus::setUser(RamUser *user)
{
    _user = user;
    emit changed(this);
}

RamState *RamStatus::state() const
{
    return _state;
}

void RamStatus::setState(RamState *state)
{
    _state = state;
    emit changed(this);
}

QString RamStatus::comment() const
{
    return _comment;
}

void RamStatus::setComment(QString comment)
{
    _comment = comment;
    emit changed(this);
}

int RamStatus::version() const
{
    return _version;
}

void RamStatus::setVersion(int version)
{
    _version = version;
    emit changed(this);
}

RamStep *RamStatus::step() const
{
    return _step;
}

void RamStatus::setStep(RamStep *step)
{
    _step = step;
    emit changed(this);
}

void RamStatus::update()
{

}

QDateTime RamStatus::date() const
{
    return _date;
}

void RamStatus::setDate(const QDateTime &date)
{
    _date = date;
    emit changed(this);
}
