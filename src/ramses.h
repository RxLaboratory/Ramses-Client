#ifndef RAMSES_H
#define RAMSES_H

#include "duqf-app/app-utils.h"
#include "dbinterface.h"
#include "ramuser.h"
#include "ramproject.h"
#include "ramstep.h"
#include "dbisuspender.h"

#include <QObject>

class Ramses : public QObject
{
    Q_OBJECT
public:
    static Ramses *instance();
    ~Ramses();
    void login(QString username, QString password);
    void logout();
    bool isConnected() const;
    // Users
    QList<RamUser *> users() const;
    RamUser *currentUser() const;
    RamUser *defaultUser() const;
    RamUser *createUser();
    void removeUser(QString uuid);
    bool isAdmin();
    // Projects
    QList<RamProject *> projects() const;
    RamProject *createProject();
    void removeProject(QString uuid);
    // Template Steps
    QList<RamStep *> templateSteps() const;
    RamStep *createTemplateStep();
    void removeTemplateStep(QString uuid);

signals:
    void loggedIn(RamUser*);
    void loggedOut();
    void newUser(RamUser *user);
    void newProject(RamProject *project);
    void newTemplateStep(RamStep *step);

protected:
    static Ramses *_instance;

private slots:
    void update();
    void newData(QJsonObject data);
    //users
    void gotUsers(QJsonArray users);
    void userDestroyed(QObject *o);
    //projects
    void gotProjects(QJsonArray projects);
    void projectDestroyed(QObject *o);
    //template steps
    void gotTemplateSteps(QJsonArray steps);
    void templateStepDestroyed(QObject *o);
    //TODO This should be modified when implementing offline version
    void dbiConnectionStatusChanged(NetworkUtils::NetworkStatus s);
private:
    /**
     * @brief Ramses is the class managing all data. It is a singleton
     * @param parent
     */
    explicit Ramses(QObject *parent = nullptr);

    DBInterface *_dbi;
    void login(QJsonObject user);
    /**
     * @brief True when loogged in, false otherwise.
     */
    bool _connected;

    // Users
    QList<RamUser *> _users;
    RamUser *_currentUser;
    RamUser *_defaultUser;
    QString _currentUserShortName;

    // Projects
    QList<RamProject *> _projects;

    // Template steps
    QList<RamStep *> _templateSteps;
};

#endif // RAMSES_H
