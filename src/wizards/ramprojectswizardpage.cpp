#include "ramprojectswizardpage.h"

#include "duapp/duui.h"
#include "ramjsonprojecteditwidget.h"
#include "mainwindow.h"
#include "ramuuid.h"

RamProjectsWizardPage::RamProjectsWizardPage(RamJsonObjectModel *projects, QWidget *parent):
    QWizardPage(parent),
    _projects(projects)
{
    setupUi();
    connectEvents();
}

void RamProjectsWizardPage::reinit()
{
    if (_projects->rowCount() > 0)
        _projects->removeRows(0, _projects->rowCount());

    if (_assignList->rowCount() > 0)
        _assignList->removeRows(0, _assignList->rowCount());

    qDeleteAll(_userAssignments);
    _userAssignments.clear();

    _initialized = false;
    initializePage();
}

void RamProjectsWizardPage::initializePage()
{
    if (_initialized)
        return;
    _initialized = true;

    // List existing projects from the server
    QJsonObject obj = RamServerClient::i()->getProjects();
    if (!obj.value("success").toBool(false)) {
        QMessageBox::warning(this,
                             tr("Project list"),
                             tr("Can't get the project list from the server, sorry!") + "\n" + obj.value("message").toString()
                             );
        return;
    }

    const QJsonArray projectsArr = obj.value("content").toArray();

    for (const auto &projectVal: projectsArr) {
        QJsonObject serverObj = projectVal.toObject();
        QString dataStr = serverObj.value("data").toString();
        QJsonDocument dataDoc = QJsonDocument::fromJson(dataStr.toUtf8());
        QJsonObject projectObj = dataDoc.object();
        projectObj.insert(RamObject::KEY_Uuid, serverObj.value("uuid").toString());
        int i = _projects->rowCount();
        _projects->insertRows(i, 1);
        _projects->setData(_projects->index(i), projectObj);
    }


    // List users
    obj = RamServerClient::i()->getAllUsers();
    if (!obj.value("success").toBool(false)) {
        QMessageBox::warning(this,
                             tr("User list"),
                             tr("Can't get the user list from the server, sorry!") + "\n" + obj.value("message").toString()
                             );
    }

    const QJsonArray usersArr = obj.value("content").toArray();

    for (const auto &userVal: usersArr)
        addUserToModel(userVal, _assignList);
}

void RamProjectsWizardPage::editProject(const QModelIndex &index)
{
    QJsonObject data = index.data(Qt::EditRole).toJsonObject();
    _currentUuid = data.value("uuid").toString();
    if (_currentUuid == "")
        _currentUuid = RamUuid::generateUuidString();

    auto editor = new RamJsonProjectEditWidget( _currentUuid, this);
    editor->hideUsersTab();
    editor->setData(data);

    connect(editor, &RamJsonProjectEditWidget::dataEdited,
            this, [this, index] (const QJsonObject &obj) {
                _projects->setData(index, obj);
            });
    connect(this, &RamProjectsWizardPage::destroyed,
            editor, &RamJsonProjectEditWidget::deleteLater);

    // Get-Create user list
    _users = _userAssignments.value(_currentUuid, new RamJsonObjectModel(this));
    _userAssignments.insert(_currentUuid, _users);

    // Remove previous list
    if (ui_userList) {
        ui_userList->hide();
        ui_userList->deleteLater();
    }
    // Set a new one
    ui_userList = new DuListEditView(tr("Assigned users"), tr("user"), _users, this);
    ui_userList->setAssignList(_assignList, true);
    ui_layout->addWidget(ui_userList);

    // If the assignement list is empty, have a look at the server
    if (_users->rowCount() == 0) {
        QJsonObject rep = RamServerClient::i()->getUsers(_currentUuid);
        if (!rep.value("success").toBool(false)){
            QMessageBox::warning(
                this,
                tr("List assigned users failed."),
                rep.value("message").toString()
                );
        }
        else {
            const QJsonArray usersArr = rep.value("content").toArray();
            for (const auto &userVal: usersArr)
                addUserToModel(userVal, _users);
        }
    }

    // Show
    DuUI::appMainWindow()->setPropertiesDockWidget(
        editor,
        tr("Project"),
        ":/icons/project",
        true);
}

void RamProjectsWizardPage::setupUi()
{
    this->setTitle(tr("Projects"));
    this->setSubTitle(tr("Manage server projects."));

    ui_layout = DuUI::createBoxLayout(Qt::Horizontal);
    DuUI::centerLayout(ui_layout, this, 200);

    ui_projectsList = new DuListEditView(tr("Projects"), tr("project"), _projects, this);
    ui_layout->addWidget(ui_projectsList);

    _assignList = new RamJsonObjectModel(this);
}

void RamProjectsWizardPage::connectEvents()
{
    connect(ui_projectsList, &DuListEditView::editing,
            this, &RamProjectsWizardPage::editProject);
}

void RamProjectsWizardPage::addUserToModel(const QJsonValue &userVal, RamJsonObjectModel *model)
{
    QJsonObject serverObj = userVal.toObject();
    QString dataStr = serverObj.value("data").toString();
    QJsonDocument dataDoc = QJsonDocument::fromJson(dataStr.toUtf8());
    QJsonObject userObj = dataDoc.object();
    userObj.insert(RamObject::KEY_Uuid, serverObj.value("uuid").toString());
    userObj.insert("role", serverObj.value("role").toString());
    int i = model->rowCount();
    model->insertRows(i, 1);
    model->setData(model->index(i), userObj);
}
