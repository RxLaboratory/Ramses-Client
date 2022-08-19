#ifndef RAMABSTRACTOBJECT_H
#define RAMABSTRACTOBJECT_H

#include <QSettings>

/**
 * @brief The RamAbstractObject class is the base class for RamObject and RamObjectList
 */
class RamAbstractObject
{
public:

    // ENUMS //

    /**
     * @brief The ObjectType enum lists all types of RamObjects
     */
    enum ObjectType { Application,
                    Asset,
                    AssetGroup,
                    FileType,
                    Object,
                    Item,
                    ItemTable,
                    Pipe,
                    PipeFile,
                    Project,
                    Sequence,
                    Shot,
                    State,
                    Status,
                    Step,
                    User,
                    ObjectList,
                    StepStatusHistory,
                    ScheduleEntry,
                    ScheduleComment,
                    TemplateStep,
                    TemplateAssetGroup,
                    Ramses
                    };
    /**
     * @brief The SubFolder enum lists all predefined subfolders
     */
    enum SubFolder { NoFolder,
                   ConfigFolder,
                   AdminFolder,
                   PreProdFolder,
                   ProdFolder,
                   PostProdFolder,
                   AssetsFolder,
                   ShotsFolder,
                   ExportFolder,
                   TemplatesFolder,
                   PublishFolder,
                   VersionsFolder,
                   PreviewFolder,
                   UsersFolder,
                   ProjectsFolder,
                   TrashFolder,
                   DataFolder };
    /**
     * @brief The UserRole enum lists the available roles for users, and the level of their rights
     */
    enum UserRole { Admin = 3,
                    ProjectAdmin = 2,
                    Lead = 1,
                    Standard = 0 };

    // STATIC METHODS //

    /**
     * @brief objectTypeName gets the name of a type, as used in the Database and API classes
     * @param type
     * @return
     */
    static const QString objectTypeName(ObjectType type);
    ObjectType objectTypeFromName(QString name);

    /**
     * @brief subFolderName gets the actual name of a subfolder
     * @param folder
     * @return
     */
    static const QString subFolderName(SubFolder folder);

    // METHODS //

    RamAbstractObject(QString shortName, QString name, ObjectType type, bool isVirtual = false, bool encryptData = false);
    ~RamAbstractObject();

    bool is(RamAbstractObject *other) const;

    /**
     * @brief uuid is this object's uuid
     * @return
     */
    QString uuid() const;

    /**
     * @brief objectType the type of ramobject
     * @return
     */
    ObjectType objectType() const;
    QString objectTypeName() const;

    /**
     * @brief data gets the data from the database
     * @return
     */
    QJsonObject data() const;
    /**
     * @brief getData returns the data for a specific key
     * @return
     */
    QJsonValue getData(QString key) const;
    /**
     * @brief setData sets an entirely new data
     * @param data
     */
    void setData(QJsonObject data);
    /**
     * @brief insertData inserts or updates a value in the data
     * @param key
     * @param value
     */
    void insertData(QString key, QJsonValue value);

    /**
     * @brief shortName the identifier of the object
     * @return
     */
    virtual QString shortName() const;
    virtual void setShortName(const QString &shortName);

    /**
     * @brief name the user-friendly name of the object
     * @return
     */
    virtual QString name() const;
    void setName(const QString &name);

    /**
     * @brief comment is a comment associated to this object. Plain text or markdown
     * @return
     */
    QString comment() const;
    void setComment(const QString comment);

    QColor color() const;
    void setColor(QColor color);

    /**
     * @brief remove marks the object as removed in the database
     */
    virtual void remove();
    /**
     * @brief restore markes the object as available in the database
     */
    void restore();
    bool removed();

    /**
     * @brief settings are the settings corresponding to this object
     * @return
     */
    QSettings *settings();
    void reInitSettingsFile();

    QString path(SubFolder subFolder = NoFolder, bool create = false) const;
    QString path(SubFolder subFolder, QString subPath, bool create = false) const;
    QStringList listFiles(SubFolder subFolder = NoFolder, QString subPath = "") const;
    QList<QFileInfo> listFileInfos(SubFolder subFolder = NoFolder, QString subPath = "") const;
    QStringList listFolders(SubFolder subFolder = NoFolder) const;
    QStringList listFolders(SubFolder subFolder, QString subPath) const;
    void deleteFile(QString fileName, SubFolder folder=NoFolder) const;
    void revealFolder(SubFolder subFolder = NoFolder);
    QString previewImagePath();

protected:

    // METHODS //

    /**
     * @brief RamAbstractObject constructs an existing object from an existing uuid. This method is private, use obj(QString) to get an object from the uuid
     * @param uuid
     */
    RamAbstractObject(QString uuid, ObjectType type, bool encryptData = false);

    // Low level data handling.
    QString dataString() const;
    void setDataString(QString data);
    void createData(QString data);
    virtual QJsonObject reloadData() = 0;

    // SIGNALS in QObject instances
    virtual void emitDataChanged() {};
    virtual void emitRemoved() {};
    virtual void emitRestored() {};

    // UTILS

    // Checks if this is a valid uuid,
    // and if it exists in the DB
    static bool checkUuid(QString uuid, ObjectType type);
    /**
     * @brief folderPath the folder of this object
     * @return
     */
    virtual QString folderPath() const = 0;

    // ATTRIBUTES //

    QString m_uuid;
    ObjectType m_objectType;
    bool m_virtual = false;
    bool m_dataEncrypted = false;
    QString m_cachedData = "";

private:
    QSettings *m_settings = nullptr;
};

#endif // RAMABSTRACTOBJECT_H
