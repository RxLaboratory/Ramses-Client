#ifndef RAMOBJECT_H
#define RAMOBJECT_H

#include <QObject>
#include <QMap>
#include <QSettings>
#include <QStringBuilder>
#include <QMetaEnum>

class ObjectDockWidget;
class ObjectEditWidget;

class RamObject : public QObject
{
    Q_OBJECT
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
                    ObjectUberList,
                    StepStatusHistory,
                    ScheduleEntry,
                    ScheduleComment};
    Q_ENUM( ObjectType )
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
    Q_ENUM( SubFolder )
    /**
     * @brief The UserRole enum lists the available roles for users, and the level of their rights
     */
    enum UserRole { Admin = 3,
                    ProjectAdmin = 2,
                    Lead = 1,
                    Standard = 0 };
    Q_ENUM( UserRole )

    // STATIC //

    static const QString objectTypeName(ObjectType type);
    static const QString subFolderName(SubFolder folder);
    static RamObject *objFromName(QString shortNameOrName , ObjectType objType);

    /**
     * @brief object Gets the existing object
     * @param uuid The object uuid
     * @return the object or nullptr if it doesn't exist yet
     */
    static RamObject* obj(QString uuid);

    // OTHER //

    ~RamObject();

    QString uuid() const;

    RamObject::ObjectType objectType() const;
    QString objectTypeName() const;
    void setObjectType(ObjectType type);

    /**
     * @brief data gets the data from the database
     * @return
     */
    QJsonObject data() const;
    /**
     * @brief setData sets an entirely new data
     * @param data
     */
    void setData(QJsonObject data);
    void insertData(QString key, QJsonValue value);

    QString shortName() const;
    void setShortName(const QString &shortName);

    virtual QString name() const;
    void setName(const QString &name);

    QString comment() const;
    void setComment(const QString comment);

    int order() const;
    void setOrder(int order);

    void remove();
    void restore();

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

     QString filterUuid() const;

public slots:
    virtual void edit(bool s = true) { Q_UNUSED(s) };

signals:
    void dataChanged(RamObject *, QJsonObject);
    void removed(RamObject *);
    void restored(RamObject *);

protected:

    // STATIC //

    /**
     * @brief m_typeMeta is used to return information from the type,
     * like the corresponding string
     */
    static const QMetaEnum m_typeMeta;
    /**
     * @brief m_existingObjects The list of all existing objects.
     */
    static QMap<QString, RamObject*> m_existingObjects;

    // METHODS //

    /**
     * @brief RamObject constructs an object using its UUID and type. The UUID must exist in the database.
     * @param uuid
     * @param parent
     */
    explicit RamObject(QString uuid, ObjectType type, QObject *parent = nullptr);

    /**
     * @brief setEditWidget sets the widget used to edit this object in the UI
     * @param w
     */
    void setEditWidget(ObjectEditWidget *w );
    /**
     * @brief showEdit shows the edit widget
     * @param title
     */
    void showEdit(QString title = "");
    virtual QString folderPath() const { return QString(); };

    // ATTRIBUTES //

    bool m_removed = false;
    QString m_icon = ":/icons/asset";
    UserRole m_editRole = Admin;
    bool m_editable = true;

    QString m_uuid;
    bool m_editReady = false;

    QString m_filterUuid;

    QFrame *ui_editWidget = nullptr;

private:
    RamObject::ObjectType m_objectType = Object;
    ObjectDockWidget *m_dockWidget = nullptr;
    QSettings *m_settings = nullptr;
};

#endif // RAMOBJECT_H
