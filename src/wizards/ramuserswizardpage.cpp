#include "ramuserswizardpage.h"

#include <QMessageBox>
#include <QJsonArray>
#include <QJsonDocument>

#include "duapp/duui.h"
#include "ramjsonusereditwidget.h"
#include "ramserverclient.h"
#include "ramobject.h"
#include "mainwindow.h"

RamUsersWizardPage::RamUsersWizardPage(RamJsonObjectModel *users, bool serverEdit, QWidget *parent):
    QWizardPage(parent),
    _users(users),
    _editingServer(serverEdit)
{
    setupUi();
    connectEvents();
}

void RamUsersWizardPage::reinit()
{
    if (_users->rowCount() > 0)
        _users->removeRows(0, _users->rowCount());

    if (_assignList && _assignList->rowCount() > 0)
        _assignList->removeRows(0, _assignList->rowCount());

    _initialized = false;

    initializePage();
}

void RamUsersWizardPage::initializePage()
{
    if (_initialized)
        return;
    _initialized = true;

    if (_editingServer)
        initializeServerList();
    else
        initializeAssignList();
}

void RamUsersWizardPage::editUser(const QModelIndex &index)
{
    QJsonObject data = index.data(Qt::EditRole).toJsonObject();
    QString uuid = data.value("uuid").toString();

    auto editor = new RamJsonUserEditWidget(
        uuid == field(QStringLiteral("userUuid")).toString(),
        true,
        true,
        uuid,
        this);

    editor->setData(data);

    connect(editor, &RamJsonUserEditWidget::dataEdited,
            this, [this, index] (QJsonObject obj) {
                obj.insert("edited", true);
                _users->setData(index, obj);
            });
    connect(this, &RamUsersWizardPage::destroyed,
            editor, &RamJsonUserEditWidget::deleteLater);

    // Show
    DuUI::appMainWindow()->setPropertiesDockWidget(
        editor,
        tr("User"),
        ":/icons/user",
        true);
}

void RamUsersWizardPage::setupUi()
{
    this->setTitle(tr("Users"));
    if (_editingServer)
        this->setSubTitle(tr("Manage server users."));
    else
        this->setSubTitle(tr("Assign users working on this project."));

    auto layout = DuUI::createBoxLayout(Qt::Vertical);
    DuUI::centerLayout(layout, this, 200);

    ui_userList = new DuListEditView(tr("Users"), tr("user"), _users, this);
    layout->addWidget(ui_userList);
}

void RamUsersWizardPage::connectEvents()
{
    connect(ui_userList, &DuListEditView::editing,
            this, &RamUsersWizardPage::editUser);
}

void RamUsersWizardPage::initializeAssignList()
{
    if (!_assignList) {
        _assignList = new RamJsonObjectModel(this);
        ui_userList->setAssignList(_assignList);
    }

    const QJsonArray usersArr = getServerUsers();

    for (const auto &userVal: usersArr)
        addUserToModel(userVal, _assignList);

}

void RamUsersWizardPage::initializeServerList()
{
    const QJsonArray usersArr = getServerUsers();

    for (const auto &userVal: usersArr)
        addUserToModel(userVal, _users);
}

void RamUsersWizardPage::addUserToModel(const QJsonValue &userVal, RamJsonObjectModel *model)
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

QJsonArray RamUsersWizardPage::getServerUsers()
{
    // List existing users from the server
    QJsonObject obj = RamServerClient::i()->getAllUsers();
    if (!obj.value("success").toBool(false)) {
        QMessageBox::warning(this,
                             tr("User list"),
                             tr("Can't get the user list from the server, sorry!") + "\n" + obj.value("message").toString()
                             );
        return QJsonArray();
    }

    return obj.value("content").toArray();
}
