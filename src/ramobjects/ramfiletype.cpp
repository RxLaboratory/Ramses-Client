#include "ramfiletype.h"

RamFileType::RamFileType(QString shortName, QString name, QStringList extensions, QString uuid, QObject *parent):
    RamObject(shortName, name, uuid, parent)
{
    setObjectType(FileType);
    _extensions = extensions;
    _dbi->createFileType(_shortName, _name, _extensions, _uuid);

    this->setObjectName( "RamFileType" );
}

RamFileType::RamFileType(QString shortName, QString name, QString extensions, QString uuid, QObject *parent):
    RamObject(shortName, name, uuid, parent)
{
    setObjectType(FileType);
    setExtensions(extensions);
    _dbi->createFileType(_shortName, _name, _extensions, _uuid);

    this->setObjectName( "RamFileType" );
}

RamFileType::~RamFileType()
{
    _dbi->removeFileType(_uuid);
}

void RamFileType::setExtensions(QString extensions)
{
    _dirty = true;
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
    if (!_dirty) return;
    RamObject::update();
    _dbi->updateFileType(_uuid, _shortName, _name, _extensions, _previewable);
}

bool RamFileType::isPreviewable() const
{
    return _previewable;
}

void RamFileType::setPreviewable(bool previewable)
{
    if (previewable == _previewable) return;
    _dirty = true;
    _previewable = previewable;
    emit changed(this);
}
