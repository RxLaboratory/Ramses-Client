#include "ramabstractobject.h"

#include "ramuuid.h"
#include "dbinterface.h"
#include "ramses.h"
#include "ramnamemanager.h"
#include "datacrypto.h"
#include "duqf-app/app-style.h"

// STATIC //

QMap<QString, RamAbstractObject*> RamAbstractObject::m_allObjects = QMap<QString, RamAbstractObject*>();

const QString RamAbstractObject::objectTypeName(ObjectType type)
{
    switch (type)
    {
    case Application: return "RamApplication";
    case Asset: return "RamAsset";
    case AssetGroup: return "RamAssetGroup";
    case FileType: return "RamFileType";
    case Object: return "RamObject";
    case Item: return "RamItem";
    case Pipe: return "RamPipe";
    case PipeFile: return "RamPipeFile";
    case Project: return "RamProject";
    case Sequence: return "RamSequence";
    case Shot: return "RamShot";
    case State: return "RamState";
    case Status: return "RamStatus";
    case Step: return "RamStep";
    case User: return "RamUser";
    case ObjectList: return "RamObjectList";
    case StepStatusHistory: return "RamStepStatusHistory";
    case ScheduleEntry: return "RamScheduleEntry";
    case ScheduleComment: return "RamScheduleComment";
    case TemplateStep: return "RamTemplateStep";
    case TemplateAssetGroup: return "RamTemplateAssetGroup";
    case Ramses: return "Ramses";
    case ItemTable: return "RamItemTable";
    }
    return "RamObject";
}

RamAbstractObject::ObjectType RamAbstractObject::objectTypeFromName(QString name)
{
    if (name == "RamApplication") return Application;
    if (name == "RamAsset") return Asset;
    if (name == "RamApplication") return Application;
    if (name == "RamAssetGroup") return AssetGroup;
    if (name == "RamFileType") return FileType;
    if (name == "RamObject") return Object;
    if (name == "RamItem") return Item;
    if (name == "RamPipe") return Pipe;
    if (name == "RamPipeFile") return PipeFile;
    if (name == "RamProject") return Project;
    if (name == "RamSequence") return Sequence;
    if (name == "RamShot") return Shot;
    if (name == "RamState") return State;
    if (name == "RamStatus") return Status;
    if (name == "RamStep") return Step;
    if (name == "RamUser") return User;
    if (name == "RamObjectList") return ObjectList;
    if (name == "RamStepStatusHistory") return StepStatusHistory;
    if (name == "RamScheduleEntry") return ScheduleEntry;
    if (name == "RamScheduleComment") return ScheduleComment;
    if (name == "RamTemplateStep") return TemplateStep;
    if (name == "RamTemplateAssetGroup") return TemplateAssetGroup;
    if (name == "Ramses") return Ramses;
    if (name == "RamItemTable") return ItemTable;
    return Object;
}

const QString RamAbstractObject::subFolderName(SubFolder folder)
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

void RamAbstractObject::setObjectData(QString uuid, QString dataStr)
{
    // TODO use the localdbinterface instead? (we need the type)

    RamAbstractObject *obj = nullptr;
    if (m_allObjects.contains(uuid)) obj = m_allObjects[uuid];
    if (!obj) return;

    obj->setDataString(dataStr);
}

void RamAbstractObject::setObjectData(QString uuid, QJsonObject data)
{
    // TODO use the localdbinterface instead? (we need the type)

    RamAbstractObject *obj = nullptr;
    if (m_allObjects.contains(uuid)) obj = m_allObjects[uuid];
    if (!obj) return;

    obj->setData(data);
}

QJsonObject RamAbstractObject::getObjectData(QString uuid)
{
    // TODO use the localdbinterface instead? (we need the type)

    RamAbstractObject *obj = nullptr;
    if (m_allObjects.contains(uuid)) obj = m_allObjects[uuid];
    if (!obj) return QJsonObject();

    return obj->data();
}

QString RamAbstractObject::getObjectDataString(QString uuid)
{
    // TODO use the localdbinterface instead? (we need the type)

    RamAbstractObject *obj = nullptr;
    if (m_allObjects.contains(uuid)) obj = m_allObjects[uuid];
    if (!obj) return "{}";

    return obj->dataString();
}

QString RamAbstractObject::getObjectPath(QString uuid)
{
    RamAbstractObject *obj = nullptr;
    if (m_allObjects.contains(uuid)) obj = m_allObjects[uuid];
    if (!obj) return "";

    return obj->path();
}

const QString RamAbstractObject::uuidFromPath(QString path, ObjectType type)
{
    if (!path.endsWith("/")) path = path + "/";

    // Check the path of all existing ramObjects
    QMapIterator<QString, RamAbstractObject*> i = QMapIterator<QString, RamAbstractObject*>(m_allObjects);
    // We'll keep the closest match
    while (i.hasNext())
    {
        i.next();
        RamAbstractObject *o = i.value();
        if (o->objectType() != type) continue;

        // If we have the same starting path, that's the one!
        QString testPath = o->path();
        if (!testPath.endsWith("/")) testPath = testPath + "/";

        if (path.startsWith(testPath)) return o->uuid();
    }
    return "";
}

// PUBLIC //

RamAbstractObject::RamAbstractObject(QString shortName, QString name, ObjectType type, bool isVirtual, bool encryptData)
{
    m_uuid = RamUuid::generateUuidString(shortName + name);
    m_objectType = type;
    m_dataEncrypted = encryptData;
    m_virtual = isVirtual;

    if (m_virtual) return;

    // Create in the database
    QJsonObject data;
    data.insert("shortName", shortName);
    data.insert("name", name);
    data.insert("comment", "");
    QJsonDocument doc(data);
    createData(doc.toJson(QJsonDocument::Compact));

    construct();
}

RamAbstractObject::~RamAbstractObject()
{
    m_settings->deleteLater();
}

bool RamAbstractObject::is(RamAbstractObject *other) const
{
    if (!other) return false;
    return this->uuid() == other->uuid();
}

QString RamAbstractObject::uuid() const
{
    return m_uuid;
}

RamAbstractObject::ObjectType RamAbstractObject::objectType() const
{
    return m_objectType;
}

QString RamAbstractObject::objectTypeName() const
{
    return RamAbstractObject::objectTypeName(m_objectType);
}

QJsonObject RamAbstractObject::data() const
{
    QString dataStr = dataString();
    if (dataStr == "") return QJsonObject();

    QJsonDocument doc = QJsonDocument::fromJson(dataStr.toUtf8());
    return doc.object();
}

QJsonValue RamAbstractObject::getData(QString key) const
{
    return data().value(key);
}

void RamAbstractObject::setData(QJsonObject data)
{
    QJsonDocument doc = QJsonDocument(data);
    setDataString(doc.toJson(QJsonDocument::Compact));
}

void RamAbstractObject::insertData(QString key, QJsonValue value)
{
    // Update data before inserting
    QJsonObject d = data();
    d.insert(key, value);
    setData(d);
}

QString RamAbstractObject::shortName() const
{
    return getData("shortName").toString("UNKNOWN");
}

void RamAbstractObject::setShortName(const QString &shortName)
{
    insertData("shortName", shortName);
}

QString RamAbstractObject::name() const
{
    return getData("name").toString("Unnamed");
}

void RamAbstractObject::setName(const QString &name)
{
    insertData("name", name);
}

QString RamAbstractObject::comment() const
{
    return getData("comment").toString("");
}

void RamAbstractObject::setComment(const QString comment)
{
    insertData("comment", comment.trimmed());
}

QColor RamAbstractObject::color() const
{
    QString colorName = getData("color").toString("");
    if (colorName == "") return DuUI::getColor("less-light-grey");
    return QColor( colorName );
}

void RamAbstractObject::setColor(QColor color)
{
    insertData("color", color.name() );
}

void RamAbstractObject::remove()
{
    DBInterface::instance()->removeObject(m_uuid, objectTypeName());
    emitRemoved();
}

void RamAbstractObject::restore()
{
    DBInterface::instance()->restoreObject(m_uuid, objectTypeName());
    emitRestored();
}

bool RamAbstractObject::isRemoved()
{
    return DBInterface::instance()->isRemoved(m_uuid, objectTypeName());
}

QSettings *RamAbstractObject::settings()
{
    if (m_settings) return m_settings;

    // create settings folder and file
    QString settingsPath = path(RamAbstractObject::ConfigFolder, true) % "/" % "ramses_settings.ini";
    m_settings = new QSettings( settingsPath, QSettings::IniFormat);
    return m_settings;
}

void RamAbstractObject::reInitSettingsFile()
{
    m_settings->deleteLater();
    m_settings = nullptr;
}

QString RamAbstractObject::path(RamAbstractObject::SubFolder subFolder, bool create) const
{
    QString p = this->folderPath();
    if (p == "") return "";

    QString sub = subFolderName(subFolder);
    if (sub != "") p += "/" + sub;

    return Ramses::instance()->pathFromRamses( p, create );
}

QString RamAbstractObject::path(SubFolder subFolder, QString subPath, bool create) const
{
    QString p = path(subFolder, create);
    if (p == "") return "";

    p += "/" + subPath;

    return p;
}

QStringList RamAbstractObject::listFiles(RamObject::SubFolder subFolder, QString subPath) const
{
    QDir dir( path(subFolder) + "/" + subPath);
    QStringList files = dir.entryList( QDir::Files );
    files.removeAll( RamNameManager::MetaDataFileName );
    return files;
}

QList<QFileInfo> RamAbstractObject::listFileInfos(SubFolder subFolder, QString subPath) const
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

QStringList RamAbstractObject::listFolders(SubFolder subFolder) const
{
    QDir dir( path(subFolder) );
    QStringList folders = dir.entryList( QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name );
    return folders;
}

QStringList RamAbstractObject::listFolders(SubFolder subFolder, QString subPath) const
{
    QDir dir( path(subFolder, subPath) );
    QStringList folders = dir.entryList( QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name );
    return folders;
}

void RamAbstractObject::deleteFile(QString fileName, RamObject::SubFolder folder) const
{
    QFile file( QDir(path(folder)).filePath(fileName));

    QString trashPath = path(TrashFolder, true);

    QString destination = QDir( trashPath ).filePath(fileName);
    if (QFileInfo::exists(destination))
        if (!FileUtils::moveToTrash(destination))
            QFile::remove(destination);

    file.rename( destination );
}

void RamAbstractObject::revealFolder(RamObject::SubFolder subFolder)
{
    QString p = path(subFolder);
    if (p == "") return;
    FileUtils::openInExplorer( p, true );
}

QString RamAbstractObject::previewImagePath()
{
    QDir previewDir = path(RamObject::PreviewFolder);
    QStringList filters;
    filters << "*.jpg" << "*.png" << "*.jpeg" << "*.gif";
    QStringList images = previewDir.entryList(filters, QDir::Files );

    if (images.count() == 0) return "";

    RamNameManager nm;

    foreach(QString file, images)
    {
        if (nm.setFileName(file))
        {
            if (nm.shortName().toLower() != shortName().toLower()) continue;
            return previewDir.filePath( file );
        }
    }

    // Not found, return the first one
    return previewDir.filePath( images.at(0) );
}

// PROTECTED //

RamAbstractObject::RamAbstractObject(QString uuid, ObjectType type, bool encryptData)
{
    m_uuid = uuid;
    m_objectType = type;
    m_dataEncrypted = encryptData;

    // cache the data
    m_cachedData = dataString();

    construct();
}

void RamAbstractObject::setDataString(QString data)
{
    if (m_virtual) return;

    // Cache the data to improve performance
    m_cachedData = data;

    if (m_dataEncrypted)
    {
        data = DataCrypto::instance()->clientEncrypt( data );
    }

    /*qDebug() << ">>>";
    qDebug() << "Setting data for: " + shortName() + " (" + objectTypeName() + ")";
    qDebug() << "UUID: " + m_uuid;
    qDebug() << "DATA: " + data;
    qDebug() << ">>>";*/

    DBInterface::instance()->setObjectData(m_uuid, objectTypeName(), data);

    emitDataChanged();
}

QString RamAbstractObject::dataString() const
{
    // If we have cached the data already, return it
    if (m_cachedData != "") return m_cachedData;

    QString dataStr = DBInterface::instance()->objectData(m_uuid, objectTypeName());
    if (dataStr == "") return "";
    // Decrypt
    if (m_dataEncrypted)
    {
        dataStr = DataCrypto::instance()->clientDecrypt( dataStr );
    }

    // Cache the data to improve performance
    //m_cachedData = dataStr;

    return dataStr;
}

void RamAbstractObject::createData(QString data)
{
    if (m_virtual) return;

    // Cache the data to improve performance
    m_cachedData = data;

    if (m_dataEncrypted)
    {
        data = DataCrypto::instance()->clientEncrypt( data );
    }

    DBInterface::instance()->createObject(m_uuid, objectTypeName(), data);
}

bool RamAbstractObject::checkUuid(QString uuid, ObjectType type)
{
    QString table = objectTypeName(type);
    if (uuid == "")
        qFatal( "RamAbstractObject::get - getting empty UUID." );
    Q_ASSERT_X(uuid != "", QString("%1::get").arg(table).toUtf8(), "UUID can't be empty");

    if (uuid == "none") return false;

    // Check if the uuid exists in the DB
    if (!DBInterface::instance()->contains(uuid, table))
    {
        qCritical() << QString("%1::get - This uuid can't be found in the database: %2").arg(table, uuid);
        // Don't do anything, let the caller handle it
        return false;
    }

    return true;
}

void RamAbstractObject::construct()
{
    m_allObjects[m_uuid] = this;
}
