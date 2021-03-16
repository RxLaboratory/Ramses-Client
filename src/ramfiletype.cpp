#include "ramfiletype.h"

RamFileType::RamFileType(QString shortName, QString name, QStringList extensions, QString uuid, QObject *parent):
    RamObject(shortName, name, uuid, parent)
{
    _extensions = extensions;
    _dbi->createFileType(_shortName, _name, _extensions, _uuid);
}

RamFileType::~RamFileType()
{
    _dbi->removeFileType(_uuid);
}

void RamFileType::update()
{
    _dbi->updateFileType(_uuid, _shortName, _name, _extensions);
}
