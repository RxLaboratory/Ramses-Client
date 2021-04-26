#include "ramfiletype.h"

RamFileType::RamFileType(QString shortName, QString name, QStringList extensions, QString uuid, QObject *parent):
    RamObject(shortName, name, uuid, parent)
{
    _extensions = extensions;
    _dbi->createFileType(_shortName, _name, _extensions, _uuid);
}

RamFileType::RamFileType(QString shortName, QString name, QString extensions, QString uuid, QObject *parent):
    RamObject(shortName, name, uuid, parent)
{
    setExtensions(extensions);
    _dbi->createFileType(_shortName, _name, _extensions, _uuid);
}

RamFileType::~RamFileType()
{
    _dbi->removeFileType(_uuid);
}

void RamFileType::setExtensions(QString extensions)
{
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
    _dbi->updateFileType(_uuid, _shortName, _name, _extensions, _previewable);
}

bool RamFileType::isPreviewable() const
{
    return _previewable;
}

void RamFileType::setPreviewable(bool previewable)
{
    _previewable = previewable;
    emit changed(this);
}
