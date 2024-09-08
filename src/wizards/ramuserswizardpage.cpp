#include "ramuserswizardpage.h"

#include <QMessageBox>

#include "duapp/duui.h"
#include "ramjsonusereditwidget.h"
#include "ramserverclient.h"

RamUsersWizardPage::RamUsersWizardPage(RamJsonObjectModel *users, QWidget *parent):
    QWizardPage(parent),
    _users(users)
{
    setupUi();
    connectEvents();
}

void RamUsersWizardPage::initializePage()
{
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
    for (const auto &userVal: usersArr) {
        QJsonObject userObj = userVal.toObject();
        qDebug() << userObj.value("name");

    }
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

    connect(editor, &RamJsonUserEditWidget::dataChanged,
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

    ui_userList = new DuListModelEdit(tr("Users"), _users, this);
    layout->addWidget(ui_userList);
}

void RamUsersWizardPage::connectEvents()
{
    connect(ui_userList, &DuListModelEdit::editing,
            this, &RamUsersWizardPage::editUser);
}
