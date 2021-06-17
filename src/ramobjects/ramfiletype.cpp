#include "ramfiletype.h"

RamFileType::RamFileType(QString shortName, QString name, QStringList extensions, QString uuid, QObject *parent):
    RamObject(shortName, name, uuid, parent)
{
    setObjectType(FileType);
    _extensions = extensions;
    m_dbi->createFileType(m_shortName, m_name, _extensions, m_uuid);

    this->setObjectName( "RamFileType" );
}

RamFileType::RamFileType(QString shortName, QString name, QString extensions, QString uuid, QObject *parent):
    RamObject(shortName, name, uuid, parent)
{
    setObjectType(FileType);
    setExtensions(extensions);
    m_dbi->createFileType(m_shortName, m_name, _extensions, m_uuid);

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
    _extensions.clear();
    foreach(QString ext, exts) _extensions << ext.trimmed();

    emit changed(this);
}

QStringList RamFileType::extensions() const
{
    return _extensions;
}

void RamFileType::update()
{
    if (!m_dirty) return;
    RamObject::update();
    m_dbi->updateFileType(m_uuid, m_shortName, m_name, _extensions, _previewable);
}

RamFileType *RamFileType::fileType(QString uuid)
{
    return qobject_cast<RamFileType*>( RamObject::obj(uuid) );
}

bool RamFileType::isPreviewable() const
{
    return _previewable;
}

void RamFileType::setPreviewable(bool previewable)
{
    if (previewable == _previewable) return;
    m_dirty = true;
    _previewable = previewable;
    emit changed(this);
}
