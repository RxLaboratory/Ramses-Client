#ifndef RAMUSER_H
#define RAMUSER_H

#include "ramobject.h"

class RamUser : public RamObject
{
    Q_OBJECT
public:
    enum UserRole { Admin, Lead, Standard };
    Q_ENUM( UserRole )

    explicit RamUser(QString shortName, QString name = "", QString uuid = "", QObject *parent = nullptr);
    ~RamUser();

    UserRole role() const;
    void setRole(const UserRole &role);

    QString folderPath() const;
    void setFolderPath(const QString &folderPath);

    void update();
    void updatePassword(QString c, QString n);

signals:

private:
    UserRole _role;
    QString _folderPath;
};

#endif // RAMUSER_H

