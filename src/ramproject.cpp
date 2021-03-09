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

void RamProject::stepDestroyed(QObject *o)
{
    RamStep *s = (RamStep*)o;
    removeStep(s);
}

void RamProject::assetGroupDestroyed(QObject *o)
{
    RamAssetGroup *ag = (RamAssetGroup*)o;
    removeAssetGroup(ag);
}

QList<RamAssetGroup *> RamProject::assetGroups() const
{
    return _assetGroups;
}

RamAssetGroup *RamProject::assetGroup(QString uuid) const
{
    foreach (RamAssetGroup *ag, _assetGroups)
    {
        if (ag->uuid() == uuid) return ag;
    }
    return nullptr;
}

void RamProject::addAssetGroup(RamAssetGroup *assetGroup)
{
    _assetGroups << assetGroup;
    connect(assetGroup, &RamAssetGroup::destroyed, this, &RamProject::assetGroupDestroyed);
    emit newAssetGroup(assetGroup);
}

void RamProject::assignAssetGroup(RamAssetGroup *templateAssetGroup)
{
    RamAssetGroup *assetGroup = templateAssetGroup->createFromTemplate(_uuid);
    addAssetGroup(assetGroup);
}

void RamProject::createAssetGroup(QString shortName, QString name)
{
    RamAssetGroup *assetGroup = new RamAssetGroup(shortName, name, _uuid);
    addAssetGroup(assetGroup);
}

void RamProject::removeAssetGroup(QString uuid)
{
    for (int i = _assetGroups.count() -1; i >= 0; i--)
    {
        RamAssetGroup *ag = _assetGroups[i];
        if (ag->uuid() == uuid)
        {
            _assetGroups.removeAt(i);
            ag->deleteLater();
            emit assetGroupRemoved(uuid);
        }
    }
}

void RamProject::removeAssetGroup(RamAssetGroup *assetGroup)
{
    removeAssetGroup(assetGroup->uuid());
}

bool assetGroupSorter(RamAssetGroup *a, RamAssetGroup *b)
{
    return a->shortName() > b->shortName();
}

void RamProject::sortAssetGroups()
{
    std::sort(_assetGroups.begin(), _assetGroups.end(), assetGroupSorter);
}

QList<RamStep *> RamProject::steps() const
{
    return _steps;
}

RamStep *RamProject::step(QString uuid) const
{
    foreach (RamStep *s, _steps)
    {
        if (s->uuid() == uuid) return s;
    }
    return nullptr;
}

void RamProject::addStep(RamStep *step)
{
    _steps << step;
    connect(step, &RamStep::destroyed, this, &RamProject::stepDestroyed);
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
            emit stepRemoved(uuid);
        }
    }
}

void RamProject::removeStep(RamStep *step)
{
    removeStep(step->uuid());
}

bool stepsSorter( RamStep *a, RamStep *b)
{
    return a->order() > b->order();
}

void RamProject::sortSteps()
{
    std::sort(_steps.begin(), _steps.end(), stepsSorter);
}
