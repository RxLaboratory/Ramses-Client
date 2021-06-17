#include "rampipefile.h"

RamPipeFile::RamPipeFile(QString shortName, QString projectUuid, QObject *parent) :
    RamObject(shortName, "", "", parent)
{
    this->setObjectType(PipeFile);
    m_projectUuid = projectUuid;
    m_dbi->createPipeFile(m_shortName, projectUuid, "", m_uuid, "");
}

RamPipeFile::RamPipeFile(QString uuid, QObject *parent):
    RamObject(uuid, parent)
{
    this->setObjectType(PipeFile);
}

RamPipeFile::~RamPipeFile()
{
    m_dbi->removePipeFile(m_uuid);
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
    m_dbi->updatePipeFile(m_uuid, m_shortName, ft, "" );
}

const QString &RamPipeFile::projectUuid() const
{
    return m_projectUuid;
}

void RamPipeFile::setProjectUuid(const QString &newProjectUuid)
{
    m_projectUuid = newProjectUuid;
}

RamPipeFile *RamPipeFile::pipeFile(QString uuid)
{
    return qobject_cast<RamPipeFile*>( RamObject::obj(uuid) );
}
