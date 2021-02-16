#include "ramproject.h"

RamProject::RamProject(QString shortName, QString name, QString uuid, QString path, QObject *parent):
    RamObject(shortName, name, uuid, path, parent)
{
    _dbi->createProject(_shortName, _name, _uuid);
}

RamProject::~RamProject()
{
    _dbi->removeProject(_uuid);
}

void RamProject::update()
{
    _dbi->updateProject(_uuid, _shortName, _name, _folderPath);
}
