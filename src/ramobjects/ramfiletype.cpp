#include "ramfiletype.h"

#include "ramses.h"
#include "filetypeeditwidget.h"


RamFileType::RamFileType(QString shortName, QString name, QStringList extensions, QString uuid):
    RamObject(shortName, name, uuid, Ramses::instance())
{
    setObjectType(FileType);
    m_extensions = extensions;
    m_dbi->createFileType(m_shortName, m_name, m_extensions, m_uuid);

    this->setObjectName( "RamFileType" );
}

RamFileType::RamFileType(QString shortName, QString name, QString extensions, QString uuid):
    RamObject(shortName, name, uuid, Ramses::instance())
{
    setObjectType(FileType);
    setExtensions(extensions);
    m_dbi->createFileType(m_shortName, m_name, m_extensions, m_uuid);

    this->setObjectName( "RamFileType" );
}

RamFileType::~RamFileType()
{
    m_dbi->removeFileType(m_uuid);
}

void RamFileType::setExtensions(QString extensions)
{
    m_dirty = true;
    QStringList exts = extensions.split(",");
    m_extensions.clear();
    foreach(QString ext, exts)
    {
        if (ext.startsWith(".")) ext = ext.remove(0, 1);
        m_extensions << ext.trimmed();
    }

    emit changed(this);
}

void RamFileType::setExtensions(QStringList extensions)
{
    m_extensions << extensions;
}

QStringList RamFileType::extensions() const
{
    return m_extensions;
}

void RamFileType::update()
{
    if (!m_dirty) return;
    RamObject::update();
    m_dbi->updateFileType(m_uuid, m_shortName, m_name, m_extensions, m_previewable, m_comment);
}

RamFileType *RamFileType::fileType(QString uuid)
{
    return qobject_cast<RamFileType*>( RamObject::obj(uuid) );
}

void RamFileType::edit(bool show)
{
    if (!m_editReady)
    {
        FileTypeEditWidget *ftw = new FileTypeEditWidget(this);
        setEditWidget(ftw);
        m_editReady = true;
    }
    showEdit(show);
}

bool RamFileType::isPreviewable() const
{
    return m_previewable;
}

void RamFileType::setPreviewable(bool previewable)
{
    if (previewable == m_previewable) return;
    m_dirty = true;
    m_previewable = previewable;
    emit changed(this);
}
