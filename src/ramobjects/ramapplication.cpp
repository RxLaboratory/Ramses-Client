#include "ramapplication.h"

#include "ramses.h"
#include "applicationeditwidget.h"

// STATIC //

RamApplication *RamApplication::getObject(QString uuid, bool constructNew)
{
    RamObject *obj = RamObject::getObject(uuid);
    if (!obj && constructNew) return new RamApplication( uuid );
    return qobject_cast<RamApplication*>( obj );
}

RamApplication::RamApplication(QString uuid):
    RamObject(uuid, ObjectType::Application)
{
    construct();

    // Populate lists
    QJsonObject d = data();

    m_nativeFileTypes = RamObjectList<RamFileType*>::getObject(d.value("nativeFileTypes").toString(), true);
    m_nativeFileTypes->setParent(this);
    m_importFileTypes = RamObjectList<RamFileType*>::getObject(d.value("importileTypes").toString(), true);
    m_importFileTypes->setParent(this);
    m_exportFileTypes = RamObjectList<RamFileType*>::getObject(d.value("exportFileTypes").toString(), true);
    m_exportFileTypes->setParent(this);
}

// PUBLIC //

RamApplication::RamApplication(QString shortName, QString name):
    RamObject(shortName, name, ObjectType::Application)
{
    construct();
    m_nativeFileTypes = new RamObjectList<RamFileType*>(shortName + "-native", name, this);
    m_importFileTypes = new RamObjectList<RamFileType*>(shortName + "-import", name, this);
    m_exportFileTypes = new RamObjectList<RamFileType*>(shortName + "-export", name, this);
    QJsonObject d = data();
    d.insert("nativeFileTypes", m_nativeFileTypes->uuid());
    d.insert("importileTypes", m_importFileTypes->uuid());
    d.insert("exportFileTypes", m_exportFileTypes->uuid());
    setData(d);
}

QString RamApplication::executableFilePath() const
{
    return data().value("executableFilePath").toString("");
}

void RamApplication::setExecutableFilePath(const QString &executableFilePath)
{
    insertData("executableFilePath", executableFilePath);
}

RamObjectList<RamFileType *> *RamApplication::nativeFileTypes() const
{
    return m_nativeFileTypes;
}

RamObjectList<RamFileType *> *RamApplication::importFileTypes() const
{
    return m_importFileTypes;
}

RamObjectList<RamFileType *> *RamApplication::exportFileTypes() const
{
    return m_exportFileTypes;
}

bool RamApplication::canExportFileType(RamFileType *ft) const
{
    if (m_nativeFileTypes->contains(ft)) return true;
    if (m_exportFileTypes->contains(ft)) return true;
    return false;
}

bool RamApplication::canExportFileType(QString extension) const
{
    for (int i = 0; i < m_nativeFileTypes->rowCount(); i++)
    {
        RamFileType *ft = (RamFileType*)m_nativeFileTypes->at(i);
        if (ft->extensions().contains(extension,Qt::CaseInsensitive)) return true;
    }
    for (int i = 0; i < m_exportFileTypes->rowCount(); i++)
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
    for (int i = 0; i < m_nativeFileTypes->rowCount(); i++)
    {
        RamFileType *ft = (RamFileType*)m_nativeFileTypes->at(i);
        if (ft->extensions().contains(extension,Qt::CaseInsensitive)) return true;
    }
    for (int i = 0; i < m_importFileTypes->rowCount(); i++)
    {
        RamFileType *ft = (RamFileType*)m_importFileTypes->at(i);
        if (ft->extensions().contains(extension, Qt::CaseInsensitive)) return true;
    }
    return false;
}

bool RamApplication::canOpen(QString filePath) const
{
    for(int i =0; i < m_nativeFileTypes->rowCount(); i++)
    {
        RamFileType *ft = qobject_cast<RamFileType*>( m_nativeFileTypes->at(i) );
        if (ft->check( filePath )) return true;
    }
    return false;
}

bool RamApplication::open(QString filePath) const
{
    if(!canOpen(filePath)) return false;

    QString execFilePath = executableFilePath();

    if (execFilePath == "") return false;

    return QProcess::startDetached(execFilePath, QStringList(filePath));
}

// PUBLIC SLOTS //

void RamApplication::unassignFileType(RamFileType *ft)
{
    if (!ft) return;
    m_nativeFileTypes->removeAll(ft);
    m_importFileTypes->removeAll(ft);
    m_exportFileTypes->removeAll(ft);
}

void RamApplication::edit(bool show)
{
    if (!ui_editWidget) setEditWidget(new ApplicationEditWidget(this));

    if (show) showEdit();
}

// PRIVATE //

void RamApplication::construct()
{
    m_icon = ":/icons/application";
    m_editRole = Admin;
}
