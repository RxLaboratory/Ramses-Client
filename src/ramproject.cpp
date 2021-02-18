#include "ramproject.h"

RamProject::RamProject(QString shortName, QString name, QString uuid, QObject *parent):
    RamObject(shortName, name, uuid, parent)
{
    _dbi->createProject(_shortName, _name, _uuid);
}

RamProject::~RamProject()
{
    _dbi->removeProject(_uuid);
}

QString RamProject::folderPath() const
{
    return _folderPath;
}

void RamProject::setFolderPath(const QString &folderPath)
{
    _folderPath = folderPath;
    emit changed();
}


void RamProject::update()
{
    QString path = _folderPath;
    if (path == "") path = "auto";
    _dbi->updateProject(_uuid, _shortName, _name, path);
}

