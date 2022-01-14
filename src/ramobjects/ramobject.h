#ifndef RAMOBJECT_H
#define RAMOBJECT_H

#include <QObject>
#include <QMap>
#include <QSettings>
#include <QStringBuilder>

#include "dbinterface.h"
#include "ramuuid.h"
#include "config.h"
#include "duqf-utils/utils.h"

class ObjectDockWidget;
class ObjectEditWidget;

class RamObject : public QObject
{
    Q_OBJECT
public:
    enum ObjectType { Application,
                    Asset,
                    AssetGroup,
                    FileType,
                    Generic,
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
                    ScheduleEntry};
    Q_ENUM( ObjectType )

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
                   TrashFolder};
    Q_ENUM( SubFolder )

    enum UserRole { Admin = 3,
                    ProjectAdmin = 2,
                    Lead = 1,
                    Standard = 0 };
    Q_ENUM( UserRole )

    explicit RamObject(QObject *parent = nullptr);
    explicit RamObject(QString uuid, QObject *parent = nullptr);
    explicit RamObject(QString shortName, QString name, QString uuid = "", QObject *parent = nullptr);

    QSettings *settings();
    void reInitSettingsFile();

    virtual QString shortName() const;
    void setShortName(const QString &shortName);

    virtual QString name() const;
    void setName(const QString &name);

    QString comment() const;
    void setComment(const QString comment);

    QString uuid() const;

    RamObject::ObjectType objectType() const;
    void setObjectType(ObjectType type);

    int order() const;
    void setOrder(int order);

    QString path(SubFolder subFolder = NoFolder, bool create = false) const;
    QString path(SubFolder subFolder, QString subPath, bool create = false) const;
    QStringList listFiles(SubFolder subFolder = NoFolder) const;
    QStringList listFiles(SubFolder subFolder, QString subPath) const;
    QStringList listFolders(SubFolder subFolder = NoFolder) const;
    QStringList listFolders(SubFolder subFolder, QString subPath) const;
    void deleteFile(QString fileName, SubFolder folder=NoFolder) const;
    void revealFolder(SubFolder subFolder = NoFolder);

    static QString subFolderName(SubFolder folder);

    QString filterUuid() const;

    bool is(const RamObject *other) const;

    static RamObject *obj(QString uuid);
    static RamObject *objFromName(QString shortNameOrName , ObjectType objType);

public slots:
    virtual void update();
    virtual void removeFromDB() {};
    virtual void remove(bool updateDB = true);
    virtual void edit(bool s = true) { Q_UNUSED(s) };

signals:
    void changed(RamObject *);
    void orderChanged(RamObject *, int p, int n);
    void orderChanged();
    void removed(RamObject *);

protected:
    DBInterface *m_dbi;
    QString m_shortName;
    QString m_name;
    QString m_icon = ":/icons/asset";
    UserRole m_editRole = Admin;
    // Check if changed to limit number of signals
    bool m_dirty;

    bool m_editable = true;

    QString m_uuid;
    QString m_comment;
    int m_order = -1;
    bool m_orderChanged = false;
    bool m_removing = false;
    bool m_editReady = false;
    QString m_filterUuid;

    static QMap<QString, RamObject*> m_existingObjects;

    void setEditWidget(ObjectEditWidget *w );
    void showEdit(QString title = "");
    virtual QString folderPath() const { return QString(); };

    QFrame *ui_editWidget = nullptr;

private:
    RamObject::ObjectType m_objectType = Generic;

    ObjectDockWidget *m_dockWidget = nullptr;

    QSettings *m_settings;

};

#endif // RAMOBJECT_H
