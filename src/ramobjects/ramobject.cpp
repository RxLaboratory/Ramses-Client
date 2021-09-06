#include "ramobject.h"
#include "objectdockwidget.h"
#include "objecteditwidget.h"
#include "mainwindow.h"

QMap<QString, RamObject*> RamObject::m_existingObjects = QMap<QString, RamObject*>();

RamObject::RamObject(QObject *parent) : QObject(parent)
{
    m_settings = nullptr;
    m_removing = false;
    m_shortName = "";
    m_name = "";
    m_uuid = RamUuid::generateUuidString(m_shortName);
    m_dbi = DBInterface::instance();
    this->setObjectName( "RamObject" );
    m_existingObjects[m_uuid] = this;
}

RamObject::RamObject(QString uuid, QObject *parent): QObject(parent)
{
    m_settings = nullptr;
    m_removing = false;
    m_shortName = "";
    m_name = "";
    m_uuid = uuid;
    m_dbi = DBInterface::instance();
    this->setObjectName( "RamObject" );
    m_existingObjects[m_uuid] = this;
}

RamObject::RamObject(QString shortName, QString name, QString uuid, QObject *parent) : QObject(parent)
{
    m_settings = nullptr;
    m_removing = false;
    m_shortName = shortName;
    m_name = name;
    if(m_name == "" ) m_name = shortName;
    if (uuid != "") m_uuid = uuid;
    else m_uuid = RamUuid::generateUuidString(m_shortName);
    m_dbi = DBInterface::instance();
    this->setObjectName( "RamObject" );
    m_existingObjects[m_uuid] = this;
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

QString RamObject::shortName() const
{
    return m_shortName;
}

void RamObject::setShortName(const QString &shortName)
{
    // Sanitize
    QString newShortName = shortName.trimmed();
    if (newShortName.startsWith(".")) newShortName = newShortName.remove(0,1);

    // Check and set
    if (newShortName == m_shortName) return;
    m_dirty  = true;
    m_shortName = shortName;
    emit changed(this);
}

QString RamObject::name() const
{
    return m_name;
}

void RamObject::setName(const QString &name)
{
    if (name == m_name) return;
    m_dirty = true;
    m_name = name;
    emit changed(this);
}

QString RamObject::comment() const
{
    return m_comment;
}

void RamObject::setComment(const QString comment)
{
    if (comment == m_comment) return;
    m_dirty = true;
    m_comment = comment;
    emit changed(this);
}

QString RamObject::uuid() const
{
    return m_uuid;
}

void RamObject::remove( bool updateDB )
{
    qDebug().noquote() << "Removing: " + m_name + " (uuid: " + m_uuid + ")";
    qDebug().noquote() << "- " + this->objectName();
    if (m_removing) return;
    qDebug().noquote() << "> Accepted";

    m_removing = true;
#ifdef DUMP_OBJECT_DEBUG
    dumpObjectInfo();
#endif
    emit removed(this);
    qDebug().noquote() << "> " + m_name + " Removed";

    if (updateDB) removeFromDB();

    this->deleteLater();
}

void RamObject::setEditWidget(ObjectEditWidget *w)
{
    ui_editWidget = w;

    m_dockWidget = new ObjectDockWidget(this);
    QFrame *f = new QFrame(m_dockWidget);
    QVBoxLayout *l = new QVBoxLayout();
    l->setContentsMargins(3,3,3,3);
    l->addWidget(w);
    f->setLayout(l);
    m_dockWidget->setWidget(f);

    MainWindow *mw = (MainWindow*)GuiUtils::appMainWindow();
    mw->addObjectDockWidget(m_dockWidget);
}

void RamObject::showEdit(bool show)
{
    if (m_dockWidget != nullptr )
        m_dockWidget->setVisible(show);
}

RamObject::ObjectType RamObject::objectType() const
{
    return _objectType;
}

void RamObject::setObjectType(RamObject::ObjectType type)
{
    _objectType = type;
}

int RamObject::order() const
{
    return m_order;
}

void RamObject::setOrder(int order)
{
    if (order == m_order) return;
    m_dirty = true;
    int previous = m_order;
    m_order = order;
    if (!m_dbi->isSuspended()) m_orderChanged = true;
    emit orderChanged(this, previous, order);
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

QStringList RamObject::listFiles(RamObject::SubFolder subFolder) const
{
    QDir dir( path(subFolder));
    QStringList files = dir.entryList( QDir::Files );
    files.removeAll( RamFileMetaDataManager::metaDataFileName() );
    return files;
}

QStringList RamObject::listFiles(RamObject::SubFolder subFolder, QString subPath) const
{
    QDir dir( path(subFolder) + "/" + subPath);
    QStringList files = dir.entryList( QDir::Files );
    files.removeAll( RamFileMetaDataManager::metaDataFileName() );
    return files;
}

QStringList RamObject::listFolders(SubFolder subFolder) const
{
    QDir dir( path(subFolder) );
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

QString RamObject::subFolderName(RamObject::SubFolder folder)
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
    case NoFolder: return "";
    }
    return "";
}

QString RamObject::filterUuid() const
{
    return m_filterUuid;
}

bool RamObject::is(const RamObject *other) const
{
    if (!other) return false;
    return other->uuid() == m_uuid;
}

RamObject *RamObject::obj(QString uuid)
{
    /*RamObject *obj = m_existingObjects.value(uuid, nullptr );
    if (!obj) obj = new RamObject(uuid);
    return obj;*/
    return m_existingObjects.value(uuid, nullptr );
}

void RamObject::update()
{
    m_dirty = false;
}
