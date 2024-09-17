#include "ramuserswizardpage.h"

#include <QMessageBox>
#include <QJsonArray>
#include <QJsonDocument>

#include "duapp/duui.h"
#include "ramjsonusereditwidget.h"
#include "ramserverclient.h"
#include "ramobject.h"
#include "mainwindow.h"

RamUsersWizardPage::RamUsersWizardPage(RamJsonObjectModel *users, QWidget *parent):
    QWizardPage(parent),
    _users(users)
{
    setupUi();
    connectEvents();
}

void RamUsersWizardPage::initializePage()
{
    if (_assignList)
        return;

    _assignList = new RamJsonObjectModel(this);

    // List existing users from the server
    QJsonObject obj = RamServerClient::i()->getAllUsers();
    if (!obj.value("success").toBool(false)) {
        QMessageBox::warning(this,
                             tr("User list"),
                             tr("Can't get the user list from the server, sorry!") + "\n" + obj.value("message").toString()
                             );
        return;
    }

    const QJsonArray usersArr = obj.value("content").toArray();
    qDebug() << usersArr;

    for (const auto &userVal: usersArr) {
        QJsonObject serverObj = userVal.toObject();
        qDebug() << serverObj.value("uuid");
        QString dataStr = serverObj.value("data").toString();
        QJsonDocument dataDoc = QJsonDocument::fromJson(dataStr.toUtf8());
        QJsonObject userObj = dataDoc.object();
        userObj.insert(RamObject::KEY_Uuid, serverObj.value("uuid").toString());
        userObj.insert("role", serverObj.value("role").toString());
        int i = _assignList->rowCount();
        _assignList->insertRows(i, 1);
        _assignList->setData(_assignList->index(i), userObj);
    }

    ui_userList->setAssignList(_assignList);
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

    editor->setData(data, uuid);

    connect(editor, &RamJsonUserEditWidget::dataEdited,
            this, [this, index] (const QJsonObject &obj) {
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
