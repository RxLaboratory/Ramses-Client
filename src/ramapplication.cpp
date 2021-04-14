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

void RamApplication::unassignFileType(RamObject *o)
{
    if (!o) return;
    unassignFileType(o->uuid());
}

void RamApplication::unassignFileType(QString uuid)
{
    unassignNativeFileType(uuid);
    unassignExportFileType(uuid);
    unassignImportFileType(uuid);
    emit fileTypeUnassigned(uuid);
}

bool RamApplication::canExportFileType(RamFileType *ft) const
{
    if (_nativeFileTypes.contains(ft)) return true;
    if (_exportFileTypes.contains(ft)) return true;
    return false;
}

bool RamApplication::canExportFileType(QString extension) const
{
    foreach(RamFileType *ft, _nativeFileTypes)
    {
        if (ft->extensions().contains(extension,Qt::CaseInsensitive)) return true;
    }
    foreach(RamFileType *ft, _exportFileTypes)
    {
        if (ft->extensions().contains(extension, Qt::CaseInsensitive)) return true;
    }
    return false;
}

bool RamApplication::canImportFileType(RamFileType *ft) const
{
    if (_nativeFileTypes.contains(ft)) return true;
    if (_importFileTypes.contains(ft)) return true;
    return false;
}

bool RamApplication::canImportFileType(QString extension) const
{
    foreach(RamFileType *ft, _nativeFileTypes)
    {
        if (ft->extensions().contains(extension,Qt::CaseInsensitive)) return true;
    }
    foreach(RamFileType *ft, _importFileTypes)
    {
        if (ft->extensions().contains(extension, Qt::CaseInsensitive)) return true;
    }
    return false;
}

QList<RamFileType *> RamApplication::nativeFileTypes() const
{
    return _nativeFileTypes;
}

void RamApplication::clearNativeFileTypes()
{
    _nativeFileTypes.clear();
}

void RamApplication::assignNativeFileType(RamFileType * const ft)
{
    if (!ft) return;
    _nativeFileTypes << ft;
    _dbi->assignFileType(_uuid, ft->uuid(), "native");
    connect(ft, SIGNAL(removed(RamObject*)), this, SLOT(unassignFileType(RamObject*)));
    emit fileTypeAssigned(ft);
    emit nativeFileTypeAssigned(ft);
}

void RamApplication::unassignNativeFileType(RamObject *ft)
{
    unassignNativeFileType(ft->uuid());
}

void RamApplication::unassignNativeFileType(QString uuid)
{
    _dbi->unassignFileType(_uuid, uuid, "native");
    for (int i = _nativeFileTypes.count() -1; i >= 0; i--)
    {
        RamFileType *ft = _nativeFileTypes[i];
        if (ft->uuid() == uuid)
        {
            _nativeFileTypes.removeAt(i);
            emit nativeFileTypeUnassigned(uuid);
        }
    }
}

QList<RamFileType *> RamApplication::importFileTypes() const
{
    return _importFileTypes;
}

void RamApplication::clearImportFileTypes()
{
    _importFileTypes.clear();
}

void RamApplication::assignImportFileType(RamFileType * const ft)
{
    if (!ft) return;
    _importFileTypes << ft;
    _dbi->assignFileType(_uuid, ft->uuid(), "import");
    connect(ft, SIGNAL(removed(RamObject*)), this, SLOT(unassignFileType(RamObject*)));
    emit fileTypeAssigned(ft);
    emit importFileTypeAssigned(ft);
}

void RamApplication::unassignImportFileType(RamObject *ft)
{
    unassignImportFileType(ft->uuid());
}

void RamApplication::unassignImportFileType(QString uuid)
{
    _dbi->unassignFileType(_uuid, uuid, "import");
    for (int i = _importFileTypes.count() -1; i >= 0; i--)
    {
        RamFileType *ft = _importFileTypes[i];
        if (ft->uuid() == uuid)
        {
            _importFileTypes.removeAt(i);
            emit importFileTypeUnassigned(uuid);
        }
    }
}

QList<RamFileType *> RamApplication::exportFileTypes() const
{
    return _exportFileTypes;
}

void RamApplication::clearExportFileTypes()
{
    _exportFileTypes.clear();
}

void RamApplication::assignExportFileType(RamFileType * const ft)
{
    if (!ft) return;
    _exportFileTypes << ft;
    _dbi->assignFileType(_uuid, ft->uuid(), "export");
    connect(ft, SIGNAL(removed(RamObject*)), this, SLOT(unassignFileType(RamObject*)));
    emit fileTypeAssigned(ft);
    emit exportFileTypeAssigned(ft);
}

void RamApplication::unassignExportFileType(RamObject *ft)
{
    unassignExportFileType(ft->uuid());
}

void RamApplication::unassignExportFileType(QString uuid)
{
    _dbi->unassignFileType(_uuid, uuid, "export");
    for (int i = _exportFileTypes.count() -1; i >= 0; i--)
    {
        RamFileType *ft = _exportFileTypes[i];
        if (ft->uuid() == uuid)
        {
            _exportFileTypes.removeAt(i);
            emit exportFileTypeUnassigned(uuid);
        }
    }
}
