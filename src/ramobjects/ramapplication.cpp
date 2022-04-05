#include "ramapplication.h"

#include "ramses.h"
#include "applicationeditwidget.h"

RamApplication::RamApplication(QString shortName, QString name, QString executableFilePath, QString uuid):
    RamObject(shortName, name, uuid, Ramses::instance())
{
    m_icon = ":/icons/application";
    m_editRole = Admin;

    m_nativeFileTypes = new RamObjectList(this);
    m_importFileTypes = new RamObjectList(this);
    m_exportFileTypes = new RamObjectList(this);

    setObjectType(Application);
    m_executableFilePath = executableFilePath;
    m_dbi->createApplication(m_shortName, m_name, m_executableFilePath, m_uuid);

    connect(m_nativeFileTypes, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(nativeFileTypeAssigned(QModelIndex,int,int)));
    connect(m_nativeFileTypes, SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)), this, SLOT(nativeFileTypeUnassigned(QModelIndex,int,int)));

    connect(m_importFileTypes, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(importFileTypeAssigned(QModelIndex,int,int)));
    connect(m_importFileTypes, SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)), this, SLOT(importFileTypeUnassigned(QModelIndex,int,int)));

    connect(m_exportFileTypes, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(exportFileTypeAssigned(QModelIndex,int,int)));
    connect(m_exportFileTypes, SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)), this, SLOT(exportFileTypeUnassigned(QModelIndex,int,int)));

    this->setObjectName("RamApplication");
}

RamApplication::~RamApplication()
{

}

QString RamApplication::executableFilePath() const
{
    return m_executableFilePath;
}

void RamApplication::setExecutableFilePath(const QString &executableFilePath)
{
    if (executableFilePath == m_executableFilePath) return;
    m_dirty = true;
    m_executableFilePath = executableFilePath;

    emit changed(this);
}

void RamApplication::update()
{
    if (!m_dirty) return;
    RamObject::update();
    m_dbi->updateApplication(m_uuid, m_shortName, m_name, m_executableFilePath, m_comment);
}

void RamApplication::unassignFileType(RamObject *o)
{
    if (!o) return;
    m_nativeFileTypes->removeAll(o);
    m_importFileTypes->removeAll(o);
    m_exportFileTypes->removeAll(o);
}

void RamApplication::edit(bool show)
{
    if (!m_editReady)
    {
        ApplicationEditWidget *w = new ApplicationEditWidget(this);
        setEditWidget(w);
        m_editReady = true;
    }
    if (show) showEdit();
}

void RamApplication::removeFromDB()
{
    m_dbi->removeApplication(m_uuid);
}

void RamApplication::remove(bool updateDB)
{
    // Remove lists
    m_exportFileTypes->clear();
    m_importFileTypes->clear();
    m_nativeFileTypes->clear();

    RamObject::remove(updateDB);
}

bool RamApplication::canExportFileType(RamFileType *ft) const
{
    if (m_nativeFileTypes->contains(ft)) return true;
    if (m_exportFileTypes->contains(ft)) return true;
    return false;
}

bool RamApplication::canExportFileType(QString extension) const
{
    for (int i = 0; i < m_nativeFileTypes->count(); i++)
    {
        RamFileType *ft = (RamFileType*)m_nativeFileTypes->at(i);
        if (ft->extensions().contains(extension,Qt::CaseInsensitive)) return true;
    }
    for (int i = 0; i < m_exportFileTypes->count(); i++)
    {
        RamFileType *ft = (RamFileType*)m_exportFileTypes->at(i);
        if (ft->extensions().contains(extension, Qt::CaseInsensitive)) return true;
    }
    return false;
}

bool RamApplication::canImportFileType(RamFileType *ft) const
{
    if (m_nativeFileTypes->contains(ft)) return true;
    if (m_importFileTypes->contains(ft)) return true;
    return false;
}

bool RamApplication::canImportFileType(QString extension) const
{
    for (int i = 0; i < m_nativeFileTypes->count(); i++)
    {
        RamFileType *ft = (RamFileType*)m_nativeFileTypes->at(i);
        if (ft->extensions().contains(extension,Qt::CaseInsensitive)) return true;
    }
    for (int i = 0; i < m_importFileTypes->count(); i++)
    {
        RamFileType *ft = (RamFileType*)m_importFileTypes->at(i);
        if (ft->extensions().contains(extension, Qt::CaseInsensitive)) return true;
    }
    return false;
}

RamObjectList *RamApplication::nativeFileTypes() const
{
    return m_nativeFileTypes;
}

void RamApplication::nativeFileTypeAssigned(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent)

    for (int i = first; i <= last; i++)
    {
        RamObject *ftObj = m_nativeFileTypes->at(i);
        m_dbi->assignFileType(m_uuid, ftObj->uuid(), "native");
    }
}

void RamApplication::nativeFileTypeUnassigned(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent)

    for (int i = first; i <= last; i++)
    {
        RamObject *ftObj = m_nativeFileTypes->at(i);
        m_dbi->unassignFileType(m_uuid, ftObj->uuid(), "native");
    }
}

RamObjectList *RamApplication::importFileTypes() const
{
    return m_importFileTypes;
}

RamObjectList *RamApplication::exportFileTypes() const
{
    return m_exportFileTypes;
}

bool RamApplication::canOpen(QString filePath) const
{
    for(int i =0; i < m_nativeFileTypes->count(); i++)
    {
        RamFileType *ft = qobject_cast<RamFileType*>( m_nativeFileTypes->at(i) );
        if (ft->check( filePath )) return true;
    }
    return false;
}

bool RamApplication::open(QString filePath) const
{
    if(!canOpen(filePath)) return false;

    if (m_executableFilePath == "") return false;

    return QProcess::startDetached(m_executableFilePath, QStringList(filePath));
}

RamApplication *RamApplication::application(QString uuid)
{
    return qobject_cast<RamApplication*>( RamObject::obj(uuid) );
}

void RamApplication::importFileTypeAssigned(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent)

    for (int i = first; i <= last; i++)
    {
        RamObject *ftObj = m_importFileTypes->at(i);
        m_dbi->assignFileType(m_uuid, ftObj->uuid(), "import");
    }
}

void RamApplication::importFileTypeUnassigned(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent)

    for (int i = first; i <= last; i++)
    {
        RamObject *ftObj = m_importFileTypes->at(i);
        m_dbi->unassignFileType(m_uuid, ftObj->uuid(), "import");
    }
}

void RamApplication::exportFileTypeAssigned(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent)

    for (int i = first; i <= last; i++)
    {
        RamObject *ftObj = m_exportFileTypes->at(i);
        m_dbi->assignFileType(m_uuid, ftObj->uuid(), "export");
    }
}

void RamApplication::exportFileTypeUnassigned(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent)

    for (int i = first; i <= last; i++)
    {
        RamObject *ftObj = m_exportFileTypes->at(i);
        m_dbi->unassignFileType(m_uuid, ftObj->uuid(), "export");
    }
}
