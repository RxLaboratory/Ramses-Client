#ifndef RAMUSER_H
#define RAMUSER_H

#include <QStringBuilder>

#include "ramobject.h"

class RamObjectList;
class RamStep;

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

    QSettings *userSettings() const;

    RamObjectList *schedule() const;
    bool isStepAssigned(RamStep *step) const;

    static RamUser *user(QString uuid);

public slots:
    void update() override;
    virtual void edit(bool show = true) override;
    virtual void removeFromDB() override;

private slots:
    void scheduleChanged(RamObject *entryObj);

protected:
    virtual QString folderPath() const override;   

private:
    UserRole m_role;
    QString m_folderPath;
    QSettings *m_userSettings;
    RamObjectList *m_schedule;
};

#endif // RAMUSER_H

