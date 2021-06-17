#include "ramapplication.h"

RamApplication::RamApplication(QString shortName, QString name, QString executableFilePath, QString uuid, QObject *parent):
    RamObject(shortName, name, uuid, parent)
{
    _nativeFileTypes = new RamObjectList(this);
    _importFileTypes = new RamObjectList(this);
    _exportFileTypes = new RamObjectList(this);

    setObjectType(Application);
    _executableFilePath = executableFilePath;
    m_dbi->createApplication(m_shortName, m_name, _executableFilePath, m_uuid);

    connect(_nativeFileTypes, &RamObjectList::objectRemoved, this, &RamApplication::nativeFileTypeUnassigned);
    connect(_importFileTypes, &RamObjectList::objectRemoved, this, &RamApplication::importFileTypeUnassigned);
    connect(_exportFileTypes, &RamObjectList::objectRemoved, this, &RamApplication::exportFileTypeUnassigned);
    connect(_nativeFileTypes, &RamObjectList::objectAdded, this, &RamApplication::nativeFileTypeAssigned);
    connect(_importFileTypes, &RamObjectList::objectAdded, this, &RamApplication::importFileTypeAssigned);
    connect(_exportFileTypes, &RamObjectList::objectAdded, this, &RamApplication::exportFileTypeAssigned);

    this->setObjectName("RamApplication");
}

RamApplication::~RamApplication()
{
    m_dbi->removeApplication(m_uuid);
}

QString RamApplication::executableFilePath() const
{
    return _executableFilePath;
}

void RamApplication::setExecutableFilePath(const QString &executableFilePath)
{
    if (executableFilePath == _executableFilePath) return;
    m_dirty = true;
    _executableFilePath = executableFilePath;

    emit changed(this);
}

void RamApplication::update()
{
    if (!m_dirty) return;
    RamObject::update();
    m_dbi->updateApplication(m_uuid, m_shortName, m_name, _executableFilePath);
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
    m_dbi->assignFileType(m_uuid, ft->uuid(), "native");
}

void RamApplication::nativeFileTypeUnassigned(RamObject *ft)
{
    m_dbi->unassignFileType(m_uuid, ft->uuid(), "native");
}

RamObjectList *RamApplication::importFileTypes() const
{
    return _importFileTypes;
}

RamObjectList *RamApplication::exportFileTypes() const
{
    return _exportFileTypes;
}

RamApplication *RamApplication::application(QString uuid)
{
    return qobject_cast<RamApplication*>( RamObject::obj(uuid));
}

void RamApplication::importFileTypeAssigned(RamObject * const ft)
{
    if (!ft) return;
    m_dbi->assignFileType(m_uuid, ft->uuid(), "import");
}

void RamApplication::importFileTypeUnassigned(RamObject *ft)
{
    m_dbi->unassignFileType(m_uuid, ft->uuid(), "import");
}

void RamApplication::exportFileTypeAssigned(RamObject * const ft)
{
    if (!ft) return;
    m_dbi->assignFileType(m_uuid, ft->uuid(), "export");
}

void RamApplication::exportFileTypeUnassigned(RamObject *ft)
{
    m_dbi->unassignFileType(m_uuid, ft->uuid(), "export");
}
