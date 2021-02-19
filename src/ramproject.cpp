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

void RamProject::stepDesstroyed(QObject *o)
{
    RamStep *s = (RamStep*)o;
    removeStep(s);
}

QList<RamStep *> RamProject::steps() const
{
    return _steps;
}

void RamProject::addStep(RamStep *step)
{
    _steps << step;
    connect(step, &RamStep::destroyed, this, &RamProject::stepDesstroyed);
    emit newStep(step);
}

void RamProject::assignStep(RamStep *templateStep)
{
    RamStep *step = templateStep->createFromTemplate(_uuid);
    addStep(step);
}

void RamProject::createStep(QString shortName, QString name)
{
    RamStep *step = new RamStep(shortName, name, _uuid);
    addStep(step);
}

void RamProject::removeStep(QString uuid)
{
    for (int i = _steps.count() -1; i >= 0; i--)
    {
        RamStep *s = _steps[i];
        if (s->uuid() == uuid)
        {
            _steps.removeAt(i);
            s->deleteLater();
        }
    }
}

void RamProject::removeStep(RamStep *step)
{
    removeStep(step->uuid());
}
