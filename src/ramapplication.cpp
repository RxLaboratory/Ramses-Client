#include "ramapplication.h"

RamApplication::RamApplication(QString shortName, QString name, QString executableFilePath, QString uuid, QObject *parent):
    RamObject(shortName, name, uuid, parent)
{
    _executableFilePath = executableFilePath;
    _dbi->createApplication(_shortName, _name, _executableFilePath, _uuid);
}

RamApplication::~RamApplication()
{
    _dbi->removeApplication(_uuid);
}

QString RamApplication::executableFilePath() const
{
    return _executableFilePath;
}

void RamApplication::setExecutableFilePath(const QString &executableFilePath)
{
    _executableFilePath = executableFilePath;

    emit changed(this);
}

void RamApplication::update()
{
    _dbi->updateApplication(_uuid, _shortName, _name, _executableFilePath);
}
