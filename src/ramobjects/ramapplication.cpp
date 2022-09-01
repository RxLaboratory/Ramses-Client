#include "ramapplication.h"

#include "applicationeditwidget.h"

// STATIC //

QMap<QString, RamApplication*> RamApplication::m_existingObjects = QMap<QString, RamApplication*>();

RamApplication *RamApplication::get(QString uuid)
{
    if (!checkUuid(uuid, Application)) return nullptr;

    if (m_existingObjects.contains(uuid)) return m_existingObjects.value(uuid);

    // Finally return a new instance
    return new RamApplication(uuid);
}

RamApplication *RamApplication::c(RamObject *o)
{
    return qobject_cast<RamApplication*>(o);
}

// PUBLIC //

RamApplication::RamApplication(QString shortName, QString name):
    RamObject(shortName, name, ObjectType::Application)
{
    construct();
}

QString RamApplication::executableFilePath() const
{
    return getData("executableFilePath").toString("");
}

void RamApplication::setExecutableFilePath(const QString &executableFilePath)
{
    insertData("executableFilePath", executableFilePath);
}

RamObjectList *RamApplication::nativeFileTypes() const
{
    return m_nativeFileTypes;
}

RamObjectList *RamApplication::importFileTypes() const
{
    return m_importFileTypes;
}

RamObjectList *RamApplication::exportFileTypes() const
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

// PROTECTED //

RamApplication::RamApplication(QString uuid):
    RamObject(uuid, ObjectType::Application)
{
    construct();
}

QString RamApplication::folderPath() const
{
    QFileInfo fpath( executableFilePath() );
    if (!fpath.exists()) return "";
    return fpath.absolutePath();
}

// PRIVATE //

void RamApplication::construct()
{
    m_existingObjects[m_uuid] = this;
    m_icon = ":/icons/application";
    m_editRole = Admin;
    getCreateLists();
}

void RamApplication::getCreateLists()
{
    QJsonObject d = data();

    QString uuid = d.value("nativeFileTypes").toString();
    if (uuid == "") m_nativeFileTypes = new RamObjectList("native", "Native File Types", FileType, RamObjectList::ListObject, this);
    else m_nativeFileTypes = RamObjectList::get( uuid );
    if (!m_nativeFileTypes) m_nativeFileTypes = new RamObjectList("native", "Native File Types", FileType, RamObjectList::ListObject, this);

    m_nativeFileTypes->setParent(this);
    d.insert("nativeFileTypes", m_nativeFileTypes->uuid());

    uuid = d.value("importFileTypes").toString();
    if (uuid == "") m_importFileTypes = new RamObjectList("import", "Import File Types", FileType, RamObjectList::ListObject, this);
    else m_importFileTypes = RamObjectList::get( uuid );
    if (!m_importFileTypes) m_importFileTypes = new RamObjectList("native", "Native File Types", FileType, RamObjectList::ListObject, this);

    m_importFileTypes->setParent(this);
    d.insert("importFileTypes", m_importFileTypes->uuid());

    uuid = d.value("exportFileTypes").toString();
    if (uuid == "") m_exportFileTypes = new RamObjectList("export", "Export File Types", FileType, RamObjectList::ListObject, this);
    else m_exportFileTypes = RamObjectList::get( uuid );
    if (!m_exportFileTypes) m_exportFileTypes = new RamObjectList("native", "Native File Types", FileType, RamObjectList::ListObject, this);

    m_exportFileTypes->setParent(this);
    d.insert("exportFileTypes", m_exportFileTypes->uuid());

    setData(d);
}
