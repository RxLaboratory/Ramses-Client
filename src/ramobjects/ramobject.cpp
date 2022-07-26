#include "ramobject.h"
#include "objecteditwidget.h"
#include "mainwindow.h"
#include "ramses.h"

#include "ramdatainterface/dbinterface.h"

// STATIC //
QMetaEnum const RamObject::m_typeMeta = QMetaEnum::fromType<RamObject::ObjectType>();

const QString RamObject::objectTypeName(ObjectType type)
{
    return "Ram" + QString(m_typeMeta.valueToKey(type));
}

const QString RamObject::subFolderName(SubFolder folder)
{
    switch(folder)
    {
    case AdminFolder: return "00-ADMIN";
    case ConfigFolder: return "_config";
    case PreProdFolder: return "01-PRE-PROD";
    case ProdFolder: return "02-PROD";
    case PostProdFolder: return "03-POST-PROD";
    case AssetsFolder: return "04-ASSETS";
    case ShotsFolder: return "05-SHOTS";
    case ExportFolder: return "06-EXPORT";
    case TemplatesFolder: return "Templates";
    case PublishFolder: return "_published";
    case VersionsFolder: return "_versions";
    case PreviewFolder: return "_preview";
    case UsersFolder: return "Users";
    case ProjectsFolder: return "Projects";
    case TrashFolder: return "_trash";
    case DataFolder: return "_data";
    case NoFolder: return "";
    }
    return "";
}

RamObject *RamObject::obj(QString uuid)
{
    return m_existingObjects.value( uuid, nullptr );
}

RamObject *RamObject::objFromName(QString shortNameOrName, ObjectType objType)
{
    QMapIterator<QString, RamObject*> i(m_existingObjects);
    while (i.hasNext())
    {
        i.next();
        RamObject *obj = i.value();
        if (obj->objectType() == objType)
        {
            if ( obj->shortName() == shortNameOrName ) return obj;
            if ( obj->name() == shortNameOrName ) return obj;
        }
    }
    return nullptr;
}

// PUBLIC //
RamObject::~RamObject()
{
    m_existingObjects.remove(m_uuid);
}

QString RamObject::uuid() const
{
    return m_uuid;
}

QJsonObject RamObject::data() const
{
    return DBInterface::instance()->objectData(m_uuid, objectTypeName());
}

void RamObject::setData(QJsonObject data)
{
    DBInterface::instance()->setObjectData(m_uuid, objectTypeName(), data);
    emit dataChanged(this, data);
}

void RamObject::insertData(QString key, QJsonValue value)
{
    // Update data before inserting
    QJsonObject d = data();
    d.insert(key, value);
    setData(d);
}

RamObject::ObjectType RamObject::objectType() const
{
    return m_objectType;
}

QString RamObject::objectTypeName() const
{
    return RamObject::objectTypeName(m_objectType);
}

void RamObject::setObjectType(RamObject::ObjectType type)
{
    m_objectType = type;
}

QString RamObject::shortName() const
{
    return data().value("shortName").toString("");
}

void RamObject::setShortName(const QString &shortName)
{
    insertData("shortName", shortName);
}

QString RamObject::name() const
{
    return data().value("name").toString("");
}

void RamObject::setName(const QString &name)
{
    insertData("name", name);
}

QString RamObject::comment() const
{
    return data().value("comment").toString("");
}

void RamObject::setComment(const QString comment)
{
    insertData("comment", comment);
}

int RamObject::order() const
{
    return data().value("order").toInt(0);
}

void RamObject::setOrder(int order)
{
    insertData("comment", order);
}

void RamObject::remove()
{
    m_removed = true;
    DBInterface::instance()->removeObject(m_uuid, objectTypeName());
    emit removed(this);
}

void RamObject::restore()
{
    m_removed = false;
    DBInterface::instance()->restoreObject(m_uuid, objectTypeName());
    emit restored(this);
}

QSettings *RamObject::settings()
{
    if (m_settings) return m_settings;

    // create settings folder and file
    QString settingsPath = path(RamObject::ConfigFolder, true) % "/" % "ramses_settings.ini";
    m_settings = new QSettings( settingsPath, QSettings::IniFormat, this);
    return m_settings;
}

void RamObject::reInitSettingsFile()
{
    m_settings->deleteLater();
    m_settings = nullptr;
}

QString RamObject::path(RamObject::SubFolder subFolder, bool create) const
{
    QString p = this->folderPath();
    if (p == "") return "";

    QString sub = subFolderName(subFolder);
    if (sub != "") p += "/" + sub;

    return Ramses::instance()->pathFromRamses( p, create );
}

QString RamObject::path(SubFolder subFolder, QString subPath, bool create) const
{
    QString p = path(subFolder, create);
    if (p == "") return "";

    p += "/" + subPath;

    return p;
}

QStringList RamObject::listFiles(RamObject::SubFolder subFolder, QString subPath) const
{
    QDir dir( path(subFolder) + "/" + subPath);
    QStringList files = dir.entryList( QDir::Files );
    files.removeAll( RamNameManager::MetaDataFileName );
    return files;
}

QList<QFileInfo> RamObject::listFileInfos(SubFolder subFolder, QString subPath) const
{
    QDir dir( path(subFolder) + "/" + subPath);
    QList<QFileInfo> files = dir.entryInfoList( QDir::Files );
    // Remove the ramses data file
    for (int i = files.length() - 1; i >= 0; i--)
    {
        if (files.at(i).fileName() == RamNameManager::MetaDataFileName ) files.removeAt(i);
    }
    return files;
}

QStringList RamObject::listFolders(SubFolder subFolder) const
{
    QDir dir( path(subFolder) );
    QStringList folders = dir.entryList( QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name );
    return folders;
}

QStringList RamObject::listFolders(SubFolder subFolder, QString subPath) const
{
    QDir dir( path(subFolder, subPath) );
    QStringList folders = dir.entryList( QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name );
    return folders;
}

void RamObject::deleteFile(QString fileName, RamObject::SubFolder folder) const
{
    QFile file( QDir(path(folder)).filePath(fileName));

    QString trashPath = path(TrashFolder, true);

    QString destination = QDir( trashPath ).filePath(fileName);
    if (QFileInfo::exists(destination))
        if (!FileUtils::moveToTrash(destination))
            QFile::remove(destination);

    file.rename( destination );
}

void RamObject::revealFolder(RamObject::SubFolder subFolder)
{
    QString p = path(subFolder);
    if (p == "") return;
    FileUtils::openInExplorer( p, true );
}

QString RamObject::filterUuid() const
{
    return m_filterUuid;
}

// PROTECTED //

RamObject::RamObject(QString uuid, ObjectType type, QObject *parent): QObject(parent)
{
    m_objectType = type;
    m_uuid = uuid;
    this->setObjectName( objectTypeName() );

    m_existingObjects.insert(m_uuid, this);
}

void RamObject::setEditWidget(ObjectEditWidget *w)
{
    ui_editWidget = new QFrame();
    QVBoxLayout *l = new QVBoxLayout();
    l->setContentsMargins(3,3,3,3);
    l->addWidget(w);
    ui_editWidget->setLayout(l);
}

void RamObject::showEdit(QString title)
{
    if (!ui_editWidget) return;

    MainWindow *mw = (MainWindow*)GuiUtils::appMainWindow();
    if (title == "") title = this->name();
    if (title == "") title = this->shortName();
    if (title == "") title = "Properties";

    ui_editWidget->setEnabled(false);

    if (m_editable)
    {
        RamUser *u = Ramses::instance()->currentUser();
        if (u)
        {
            ui_editWidget->setEnabled(u->role() >= m_editRole);
            if (u == this) ui_editWidget->setEnabled(true);
        }
    }

    mw->setPropertiesDockWidget( ui_editWidget, title, m_icon);
}





