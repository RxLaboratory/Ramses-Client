#include "ramstatus.h"

#include "ramitem.h"

RamStatus::RamStatus(RamUser *user, RamState *state, RamStep *step, RamItem *item):
    RamObject("", "", "", item)
{
    qDebug() << user;
    qDebug() << state;
    qDebug() << step;
    qDebug() << item;
    _user = user;
    _state = state;
    _step = step;
    _item = item;
    _completionRatio = _state->completionRatio();
    _date = QDateTime::currentDateTimeUtc();

    setObjectType(Status);

    this->setObjectName( "RamStatus" );
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

    this->setObjectName( "RamStatus" );
}

RamStatus::~RamStatus()
{
    m_dbi->removeStatus(m_uuid);
}

int RamStatus::completionRatio() const
{
    return _completionRatio;
}

void RamStatus::setCompletionRatio(int completionRatio)
{
    if (completionRatio == _completionRatio) return;
    m_dirty = true;
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
    m_dirty = true;
    _state = state;
    emit changed(this);
}

int RamStatus::version() const
{
    return _version;
}

void RamStatus::setVersion(int version)
{
    if (_version == version) return;
    m_dirty = true;
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
    if(!m_dirty) return;
    RamObject::update();
    if (!_state) return;
    m_dbi->updateStatus(m_uuid, _state->uuid(), m_comment, _version, _completionRatio);
}

QDateTime RamStatus::date() const
{
    return _date;
}

void RamStatus::setDate(const QDateTime &date)
{
    if (_date == date) return;
    m_dirty = true;
    _date = date;
    emit changed(this);
}

RamStatus *RamStatus::status(QString uuid)
{
    return qobject_cast<RamStatus*>( RamObject::obj(uuid) );
}
