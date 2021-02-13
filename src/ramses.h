#ifndef RAMSES_H
#define RAMSES_H

#include "duqf-app/app-utils.h"
#include "dbinterface.h"
#include "ramuser.h"

#include <QObject>

class Ramses : public QObject
{
    Q_OBJECT
public:
    static Ramses *instance();
    void login(QString username, QString password);
    void logout();

    QList<RamUser *> users() const;
    RamUser *currentUser() const;

    bool isConnected() const;

signals:
    void loggedIn(RamUser*);
    void loggedOut();

protected:
    static Ramses *_instance;

private slots:
    void update();
    void newData(QJsonObject data);
    void gotUsers(QJsonArray users);
private:
    /**
     * @brief Ramses is the class managing all data. It is a singleton
     * @param parent
     */
    explicit Ramses(QObject *parent = nullptr);

    DBInterface *_dbi;
    void login(QJsonObject user);
    bool _connected;

    QList<RamUser *> _users;
    RamUser *_currentUser;
    RamUser *_defaultUser;
    QString _currentUserShortName;
};

#endif // RAMSES_H
