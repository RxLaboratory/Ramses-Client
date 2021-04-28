#include "ramapplication.h"

RamApplication::RamApplication(QString shortName, QString name, QString executableFilePath, QString uuid, QObject *parent):
    RamObject(shortName, name, uuid, parent)
{
    _nativeFileTypes = new RamObjectList(this);
    _importFileTypes = new RamObjectList(this);
    _exportFileTypes = new RamObjectList(this);

    setObjectType(Application);
    _executableFilePath = executableFilePath;
    _dbi->createApplication(_shortName, _name, _executableFilePath, _uuid);

   connect(_nativeFileTypes, &RamObjectList::objectRemoved, this, &RamApplication::nativeFileTypeUnassigned);
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

void RamApplication::unassignFileType(RamObject *o)
{
    if (!o) return;
    _nativeFileTypes->removeAll(o);
    _importFileTypes->removeAll(o);
    _exportFileTypes->removeAll(o);
}

bool RamApplication::canExportFileType(RamFileType *ft) const
{
    if (_nativeFileTypes->contains(ft)) return true;
    if (_exportFileTypes->contains(ft)) return true;
    return false;
}

bool RamApplication::canExportFileType(QString extension) const
{
    for (int i = 0; i < _nativeFileTypes->count(); i++)
    {
        RamFileType *ft = (RamFileType*)_nativeFileTypes->at(i);
        if (ft->extensions().contains(extension,Qt::CaseInsensitive)) return true;
    }
    for (int i = 0; i < _exportFileTypes->count(); i++)
    {
        RamFileType *ft = (RamFileType*)_exportFileTypes->at(i);
        if (ft->extensions().contains(extension, Qt::CaseInsensitive)) return true;
    }
    return false;
}

bool RamApplication::canImportFileType(RamFileType *ft) const
{
    if (_nativeFileTypes->contains(ft)) return true;
    if (_importFileTypes->contains(ft)) return true;
    return false;
}

bool RamApplication::canImportFileType(QString extension) const
{
    for (int i = 0; i < _nativeFileTypes->count(); i++)
    {
        RamFileType *ft = (RamFileType*)_nativeFileTypes->at(i);
        if (ft->extensions().contains(extension,Qt::CaseInsensitive)) return true;
    }
    for (int i = 0; i < _importFileTypes->count(); i++)
    {
        RamFileType *ft = (RamFileType*)_importFileTypes->at(i);
        if (ft->extensions().contains(extension, Qt::CaseInsensitive)) return true;
    }
    return false;
}

RamObjectList *RamApplication::nativeFileTypes() const
{
    return _nativeFileTypes;
}

void RamApplication::nativeFileTypeAssigned(RamObject * const ft)
{
    if (!ft) return;
    _dbi->assignFileType(_uuid, ft->uuid(), "native");
}

void RamApplication::nativeFileTypeUnassigned(RamObject *ft)
{
    _dbi->unassignFileType(_uuid, ft->uuid(), "native");
}

RamObjectList *RamApplication::importFileTypes() const
{
    return _importFileTypes;
}

RamObjectList *RamApplication::exportFileTypes() const
{
    return _exportFileTypes;
}

void RamApplication::importFileTypeAssigned(RamObject * const ft)
{
    if (!ft) return;
    _dbi->assignFileType(_uuid, ft->uuid(), "import");
}

void RamApplication::importFileTypeUnassigned(RamObject *ft)
{
    _dbi->unassignFileType(_uuid, ft->uuid(), "import");
}

void RamApplication::exportFileTypeAssigned(RamObject * const ft)
{
    if (!ft) return;
    _dbi->assignFileType(_uuid, ft->uuid(), "export");
}

void RamApplication::exportFileTypeUnassigned(RamObject *ft)
{
    _dbi->unassignFileType(_uuid, ft->uuid(), "export");
}
