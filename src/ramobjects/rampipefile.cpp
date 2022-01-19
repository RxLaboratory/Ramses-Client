#include "rampipefile.h"

#include "ramproject.h"

#include "pipefileeditwidget.h"

RamPipeFile::RamPipeFile(QString shortName, RamProject *project) :
    RamObject(shortName, "", "", project)
{
    m_icon = ":/icons/file";
    m_editRole = ProjectAdmin;

    this->setObjectType(PipeFile);
    m_project = project;
    m_fileType = nullptr;
    m_dbi->createPipeFile(m_shortName, m_project->uuid(), "", m_uuid, "");
    this->setObjectName("RamPipeFile " + shortName);
}

RamPipeFile::RamPipeFile(QString uuid, QObject *parent):
    RamObject(uuid, parent)
{
    m_icon = ":/icons/file";
    m_editRole = ProjectAdmin;

    this->setObjectType(PipeFile);
    m_fileType = nullptr;
    m_project = nullptr;
    this->setObjectName("RamPipeFile");
}

RamPipeFile::~RamPipeFile()
{

}

QString RamPipeFile::name() const
{
    QString t = m_shortName;
    RamFileType *ft = m_fileType;
    if (ft) t = t + "." + ft->shortName();
    return t;
}

RamFileType *RamPipeFile::fileType() const
{
    return m_fileType;
}

void RamPipeFile::setFileType(RamFileType *newFileType)
{
    if (newFileType->is(m_fileType)) return;
    m_dirty = true;
    m_fileType = newFileType;
    emit changed(this);
}

void RamPipeFile::update()
{
    if(!m_dirty) return;
    RamObject::update();
    QString ft = "";
    if (m_fileType) ft = m_fileType->uuid();
    m_dbi->updatePipeFile(m_uuid, m_shortName, ft, "", m_comment, m_customSettings );
}

const RamProject *RamPipeFile::project() const
{
    return m_project;
}

void RamPipeFile::setProject(RamProject *project)
{
    m_project = project;
}

RamPipeFile *RamPipeFile::pipeFile(QString uuid)
{
    return qobject_cast<RamPipeFile*>( RamObject::obj(uuid) );
}

void RamPipeFile::edit(bool show)
{
    if (!m_editReady)
    {
        PipeFileEditWidget *w = new PipeFileEditWidget(this);
        setEditWidget(w);
        m_editReady = true;//*/
    }
    if (show) showEdit();
}

void RamPipeFile::removeFromDB()
{
    m_dbi->removePipeFile(m_uuid);
}

const QString &RamPipeFile::customSettings() const
{
    return m_customSettings;
}

void RamPipeFile::setCustomSettings(const QString &newCustomSettings)
{
    if (m_customSettings == newCustomSettings) return;
    m_dirty = true;
    m_customSettings = newCustomSettings;
    emit changed(this);
}
