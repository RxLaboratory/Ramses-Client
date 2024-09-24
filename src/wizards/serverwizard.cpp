#include "serverwizard.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QApplication>

#include "ramserverclient.h"
#include "ramuser.h"
#include "ramuuid.h"
#include "ramproject.h"

ServerWizard::ServerWizard(QWidget *parent, Qt::WindowFlags flags):
    QWizard(parent, flags)
{
    setOptions( options() |
               QWizard::NoBackButtonOnStartPage |
               QWizard::HaveCustomButton1 |
               QWizard::HaveCustomButton2 );

    setButtonText(QWizard::CustomButton1, tr("✓ Apply changes"));
    setButtonText(QWizard::CustomButton2, tr("⟲ Cancel all changes"));
    setButtonText(QWizard::CancelButton, tr("𐄂 Close"));

    setButtonLayout({
        QWizard::Stretch,
        QWizard::BackButton,
        QWizard::NextButton,
        QWizard::CustomButton1,
        QWizard::CustomButton2,
        QWizard::CancelButton,
    });

    setupUi();
    connectEvents();
}

void ServerWizard::done(int r)
{
    // Log out
    RamServerClient::i()->logout();
    QWizard::done(r);
}

void ServerWizard::setupUi()
{
    ui_loginPage = new LoginWizardPage(true, this);
    setPage(LoginPage, ui_loginPage );

    _users = new RamJsonObjectModel(this);
    ui_usersPage = new RamUsersWizardPage(_users, true, this);
    setPage(UsersPage, ui_usersPage);

    _projects = new RamJsonObjectModel(this);
    ui_projectsPage = new RamProjectsWizardPage(_projects, this);
    setPage(ProjectsPage, ui_projectsPage);
}

void ServerWizard::connectEvents()
{
    connect(this, &QWizard::currentIdChanged, this, &ServerWizard::pageChanged);
    connect(_users, &RamJsonObjectModel::rowsAboutToBeRemoved, this, &ServerWizard::removeUsers);
    connect(_projects, &RamJsonObjectModel::rowsAboutToBeRemoved, this, &ServerWizard::removeProjects);
    connect(this, &QWizard::customButtonClicked, this, [this](int b) {
        switch(b) {
        case QWizard::CustomButton1:
            applyChanges();
            break;
        case QWizard::CustomButton2:
            cancelChanges();
            break;
        }
    });
}

void ServerWizard::pageChanged(int id)
{
    switch(id) {
    case LoginPage:
        setButtonText(QWizard::NextButton, tr("Sign in →"));
        setButtonLayout({
            QWizard::Stretch,
            QWizard::BackButton,
            QWizard::NextButton,
            QWizard::CancelButton,
        });
        break;
    case UsersPage:
        setButtonText(QWizard::NextButton, tr("Manage projects..."));
        setButtonLayout({
            QWizard::Stretch,
            QWizard::NextButton,
            QWizard::CustomButton1,
            QWizard::CustomButton2,
            QWizard::CancelButton,
        });
        ui_usersPage->setTitle(ui_loginPage->serverAddress() + " | " + tr("Users"));
        break;
    case ProjectsPage:
        setButtonText(QWizard::BackButton, tr("Manage users..."));
        setButtonLayout({
            QWizard::Stretch,
            QWizard::BackButton,
            QWizard::NextButton,
            QWizard::CustomButton1,
            QWizard::CustomButton2,
            QWizard::CancelButton,
        });
        ui_projectsPage->setTitle(ui_loginPage->serverAddress() + " | " + tr("Projects"));
        break;
    }
}

void ServerWizard::removeUsers(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent)
    for(int i = first; i <= last; ++i){
        QJsonObject data = _users->data(_users->index(i), Qt::EditRole).toJsonObject();
        _removedUsers << data;
    }
}

void ServerWizard::removeProjects(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent)
    for(int i = first; i <= last; ++i){
        QJsonObject data = _projects->data(_projects->index(i), Qt::EditRole).toJsonObject();
        _removedProjects << data;
    }
}

void ServerWizard::applyChanges()
{
    QString userUuid = ui_loginPage->uuid();

    // Raise the time out as these operations can be longer than usual
    int currentTimeout = RamServerClient::i()->timeOut();
    if (currentTimeout < 10)
        RamServerClient::i()->setTimeout(10);

    this->setEnabled(false);
    qApp->setOverrideCursor(Qt::WaitCursor);

    // Remove users
    if (!_removedUsers.isEmpty()) {
        QStringList removedUserUuids;
        for (const auto &userObj: qAsConst(_removedUsers)) {
            // Don't remove ourselves
            QString uuid = userObj.value("uuid").toString();
            if (uuid != userUuid)
                removedUserUuids << uuid;
        }
        QJsonObject rep = RamServerClient::i()->removeUsers(removedUserUuids);
        if (!checkServerReply(rep)) {
            return returnFromApply();
        }
    }

    // Remove projects
    if (!_removedProjects.isEmpty()) {
        QStringList removedProjectUuids;
        for (const auto &projectObj: qAsConst(_removedProjects)) {
            removedProjectUuids << projectObj.value("uuid").toString();
        }
        QJsonObject rep = RamServerClient::i()->removeProjects(removedProjectUuids);
        if (!checkServerReply(rep)) {
            return returnFromApply();
        }
    }

    // Add and update users
    QJsonArray serverUsersArr;
    const QVector<QJsonObject> &jsonUsers = _users->objects();
    for(const auto &jsonUser: jsonUsers) {
        // Only users which have been edited
        if (!jsonUser.value("edited").toBool(false))
            continue;

        QString uuid = jsonUser.value(RamUser::KEY_Uuid).toString();
        // Create a UUID
        if (uuid == "") uuid = RamUuid::generateUuidString(
                jsonUser.value(RamObject::KEY_ShortName).toString()
                );

        // The object to create the user server-side
        QJsonObject userObj;
        userObj.insert("uuid", uuid);
        userObj.insert("email", jsonUser.value("email").toString());
        userObj.insert("role", jsonUser.value("role").toString("standard"));
        QJsonObject userData = jsonUser;
        userData.remove("role");
        userData.remove("email");
        userData.remove("uuid");
        userData.remove("edited");
        QJsonDocument dataDoc(userData);
        userObj.insert("data", QString::fromUtf8(dataDoc.toJson(QJsonDocument::Compact)) );
        serverUsersArr.append(userObj);
    }

    if (!serverUsersArr.isEmpty()) {
        QJsonObject rep = RamServerClient::i()->createUsers(serverUsersArr);
        if (!checkServerReply(rep)) {
            return returnFromApply();
        }
    }

    // List project assignments
    QHash<QString, RamJsonObjectModel*> assignments = ui_projectsPage->userAssignments();

    // Add and update projects
    QJsonArray serverProjectsArr;
    const QVector<QJsonObject> &jsonProjects = _projects->objects();
    for(const auto &jsonProject: jsonProjects) {
        QString uuid = jsonProject.value(RamProject::KEY_Uuid).toString();
        // Create a UUID
        if (uuid == "")
            uuid = RamUuid::generateUuidString(
                jsonProject.value(RamObject::KEY_ShortName).toString()
                );

        // The object to create the project server-side
        QJsonObject projectObj;
        projectObj.insert("uuid", uuid);
        QJsonObject projectData = jsonProject;
        projectData.remove("uuid");
        projectData.remove("edited");

        // Update the user list
        RamJsonObjectModel *usersModel = assignments.value(uuid);
        if (usersModel) {
            QJsonArray userUuids;
            for (int i = 0; i < usersModel->rowCount(); ++i) {
                QString userUuid = usersModel->data(usersModel->index(i), Qt::EditRole).toJsonObject().value("uuid").toString();
                if (userUuid != "")
                    userUuids << userUuid;
            }
            projectData.insert("users", userUuids);
        }
        // The project hasn't been edited and there's no new user, skip
        else if (!jsonProject.value("edited").toBool(false))
                continue;

        QJsonDocument dataDoc(projectData);
        projectObj.insert("data", QString::fromUtf8(dataDoc.toJson(QJsonDocument::Compact)) );
        serverProjectsArr.append(projectObj);
    }

    if (!serverProjectsArr.isEmpty()) {
        QJsonObject rep = RamServerClient::i()->createProjects(serverProjectsArr);
        if (!checkServerReply(rep)) {
            return returnFromApply();
        }
    }

    // Update user assignments
    QHashIterator<QString, RamJsonObjectModel*> it(assignments);
    while(it.hasNext()) {
        it.next();
        QString projectUuid = it.key();
        RamJsonObjectModel *usersModel = it.value();
        QStringList userUuids;
        for (int i = 0; i < usersModel->rowCount(); ++i) {
            QString userUuid = usersModel->data(usersModel->index(i), Qt::EditRole).toJsonObject().value("uuid").toString();
            if (userUuid != "")
                userUuids << userUuid;
        }

        QJsonObject rep = RamServerClient::i()->setUserAssignments(userUuids, projectUuid);
        if (!checkServerReply(rep)) {
            return returnFromApply();
        }
    }

    // Reinit with (new) server data
    returnFromApply();
}

void ServerWizard::cancelChanges()
{
    ui_usersPage->reinit();
    ui_projectsPage->reinit();
    _removedUsers.clear();
    _removedProjects.clear();
}

bool ServerWizard::checkServerReply(const QJsonObject &reply)
{
    if (!reply.value("success").toBool(false)) {
        QMessageBox::warning(
            this,
            tr("Update server data changed"),
            reply.value("message").toString("Unknown error")
            );
        return false;
    }
    return true;
}

void ServerWizard::returnFromApply()
{
    cancelChanges();
    this->setEnabled(true);
    qApp->restoreOverrideCursor();
}
