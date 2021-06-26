#include "ramstatus.h"

#include "ramitem.h"
#include "statuseditwidget.h"

RamStatus::RamStatus(RamUser *user, RamState *state, RamStep *step, RamItem *item, QString uuid):
    RamObject("", "", uuid, item)
{
    m_user = user;
    m_state = state;
    m_step = step;
    m_item = item;
    m_completionRatio = m_state->completionRatio();
    m_date = QDateTime::currentDateTimeUtc();

    setObjectType(Status);

    this->setObjectName( "RamStatus" );
}

RamStatus::~RamStatus()
{
    m_dbi->removeStatus(m_uuid);
}

int RamStatus::completionRatio() const
{
    return m_completionRatio;
}

void RamStatus::setCompletionRatio(int completionRatio)
{
    if (completionRatio == m_completionRatio) return;
    m_dirty = true;
    m_completionRatio = completionRatio;
    emit changed(this);
}

RamUser *RamStatus::user() const
{
    return m_user;
}

RamState *RamStatus::state() const
{
    return m_state;
}

void RamStatus::setState(RamState *state)
{
    if (!state && !m_state) return;
    if (state && state->is(m_state)) return;
    m_dirty = true;
    m_state = state;
    emit changed(this);
}

int RamStatus::version() const
{
    return m_version;
}

void RamStatus::setVersion(int version)
{
    if (m_version == version) return;
    m_dirty = true;
    m_version = version;
    emit changed(this);
}

RamStep *RamStatus::step() const
{
    return m_step;
}

RamItem *RamStatus::item() const
{
    return m_item;
}

void RamStatus::update()
{
    if(!m_dirty) return;
    RamObject::update();
    if (!m_state) return;
    m_dbi->updateStatus(m_uuid, m_state->uuid(), m_comment, m_version, m_completionRatio);
}

void RamStatus::edit(bool show)
{
    if (!m_editReady)
    {
        StatusEditWidget *w = new StatusEditWidget(this);
        setEditWidget(w);
        m_editReady = true;
        connect( w, SIGNAL(statusUpdated(RamState*,int,int,QString)),
                 this, SLOT(statusUpdated(RamState*,int,int,QString))
                 );
    }
    showEdit(show);
}

QString RamStatus::folderPath() const
{
    RamProject *project = m_item->project();
    QString type = "_G_";
    if (m_item->objectType() == RamObject::Shot) type = "_S_";
    else if (m_item->objectType() == RamObject::Asset) type = "_A_";
    return m_item->path() + "/" + project->shortName() + type + m_item->shortName() + "_" + m_step->shortName();
}

void RamStatus::statusUpdated(RamState *state, int completion, int version, QString comment)
{
    this->setState(state);
    this->setCompletionRatio(completion);
    this->setVersion(version);
    this->setComment(comment);
    update();
}

QDateTime RamStatus::date() const
{
    return m_date;
}

void RamStatus::setDate(const QDateTime &date)
{
    if (m_date == date) return;
    m_dirty = true;
    m_date = date;
    emit changed(this);
}

RamStatus *RamStatus::status(QString uuid)
{
    return qobject_cast<RamStatus*>( RamObject::obj(uuid) );
}
