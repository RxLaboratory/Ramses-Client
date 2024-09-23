#include "serverwizard.h"

#include "ramserverclient.h"

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
    // Remove users
    if (!_removedUsers.isEmpty()) {
        QStringList removedUserUuids;
        for (const auto &userObj: qAsConst(_removedUsers)) {
            removedUserUuids << userObj.value("uuid").toString();
        }
        QJsonObject rep = RamServerClient::i()->removeUsers(removedUserUuids);
        if (!rep.value("success").toBool(false)) {
            QMessageBox::warning(
                this,
                tr("Remove users failed"),
                rep.value("message").toString()
                );
            cancelChanges();
            return;
        }
    }

    // Remove projects
    if (!_removedProjects.isEmpty()) {
        QStringList removedProjectUuids;
        for (const auto &projectObj: qAsConst(_removedProjects)) {
            removedProjectUuids << projectObj.value("uuid").toString();
        }
        QJsonObject rep = RamServerClient::i()->removeProjects(removedProjectUuids);
        if (!rep.value("success").toBool(false)) {
            QMessageBox::warning(
                this,
                tr("Remove projects failed"),
                rep.value("message").toString()
                );
            cancelChanges();
            return;
        }
    }

    // Reinit with (new) server data
    cancelChanges();
}

void ServerWizard::cancelChanges()
{
    ui_usersPage->reinit();
    ui_projectsPage->reinit();
    _removedUsers.clear();
    _removedProjects.clear();
}
