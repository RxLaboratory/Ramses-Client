#ifndef RAMUSER_H
#define RAMUSER_H

#include "ramobject.h"

class RamUser : public RamObject
{
    Q_OBJECT
public:
    enum UserRole { Admin, Lead, Standard };
    Q_ENUM( UserRole )

    explicit RamUser(QString shortName, QString name = "", QString uuid = "", QString path = "", QObject *parent = nullptr);

    UserRole role() const;
    void setRole(const UserRole &role);

    void update();
    void updatePassword(QString c, QString n);

signals:

private:
    UserRole _role;

};

#endif // RAMUSER_H
