#include "ramstatus.h"

#include "ramitem.h"

RamStatus::RamStatus(RamUser *user, RamState *state, RamStep *step, RamItem *item):
    RamObject("", "", "", item)
{
    _user = user;
    _state = state;
    _step = step;
    _item = item;
    _completionRatio = _state->completionRatio();
    _date = QDateTime::currentDateTimeUtc();

    setObjectType(Status);
}

RamStatus::RamStatus(RamUser *user, RamState *state, RamStep *step, RamItem *item, QString uuid):
    RamObject("", "", uuid, item)
{
    _user = user;
    _state = state;
    _step = step;
    _item = item;
    _completionRatio = _state->completionRatio();
    _date = QDateTime::currentDateTimeUtc();

    setObjectType(Status);
}

RamStatus::~RamStatus()
{
    _dbi->removeStatus(_uuid);
}

int RamStatus::completionRatio() const
{
    return _completionRatio;
}

void RamStatus::setCompletionRatio(int completionRatio)
{
    if (completionRatio == _completionRatio) return;
    _dirty = true;
    _completionRatio = completionRatio;
    emit changed(this);
}

RamUser *RamStatus::user() const
{
    return _user;
}

RamState *RamStatus::state() const
{
    return _state;
}

void RamStatus::setState(RamState *state)
{
    if (!state && !_state) return;
    if (state && state->is(_state)) return;
    _dirty = true;
    _state = state;
    emit changed(this);
}

QString RamStatus::comment() const
{
    return _comment;
}

void RamStatus::setComment(QString comment)
{
    if (comment == _comment) return;
    _dirty = true;
    _comment = comment;
    emit changed(this);
}

int RamStatus::version() const
{
    return _version;
}

void RamStatus::setVersion(int version)
{
    if (_version == version) return;
    _dirty = true;
    _version = version;
    emit changed(this);
}

RamStep *RamStatus::step() const
{
    return _step;
}

RamItem *RamStatus::item() const
{
    return _item;
}

void RamStatus::update()
{
    if(!_dirty) return;
    RamObject::update();
    if (!_state) return;
    _dbi->updateStatus(_uuid, _state->uuid(), _comment, _version, _completionRatio);
}

QDateTime RamStatus::date() const
{
    return _date;
}

void RamStatus::setDate(const QDateTime &date)
{
    if (_date == date) return;
    _dirty = true;
    _date = date;
    emit changed(this);
}
