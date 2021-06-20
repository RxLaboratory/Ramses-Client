#ifndef RAMUSER_H
#define RAMUSER_H

#include "ramobject.h"

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

    QString folderPath() const;
    void setFolderPath(const QString &folderPath);

    void update();
    void updatePassword(QString c, QString n);

    static RamUser *user(QString uuid);

signals:

private:
    UserRole _role;
    QString _folderPath;
};

#endif // RAMUSER_H

