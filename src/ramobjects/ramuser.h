#ifndef RAMUSER_H
#define RAMUSER_H

#include <QStringBuilder>

#include "ramobject.h"

class RamObjectList;

class RamUser : public RamObject
{
    Q_OBJECT
public:
    enum UserRole { Admin = 3,
                    ProjectAdmin = 2,
                    Lead = 1,
                    Standard = 0 };
    Q_ENUM( UserRole )

    explicit RamUser(QString shortName, QString name = "", QString uuid = "");
    ~RamUser();

    UserRole role() const;
    void setRole(const UserRole &role);
    void setRole(const QString role);

    void setFolderPath(const QString &folderPath);
    QString defaultPath() const;
    bool pathIsDefault() const;

    void updatePassword(QString c, QString n);

    static RamUser *user(QString uuid);

    QSettings *userSettings() const;

    RamObjectList *schedule() const;

public slots:
    void update() override;
    virtual void edit(bool show = true) override;

protected:
    virtual QString folderPath() const override;

private:
    UserRole m_role;
    QString m_folderPath;
    QSettings *m_userSettings;
    RamObjectList *m_schedule;
};

#endif // RAMUSER_H

