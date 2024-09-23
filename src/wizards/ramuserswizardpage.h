#ifndef RAMUSERSWIZARDPAGE_H
#define RAMUSERSWIZARDPAGE_H

#include <QWizardPage>

#include "duwidgets/dulisteditview.h"
#include "ramjsonobjectmodel.h"

class RamUsersWizardPage : public QWizardPage
{
    Q_OBJECT
public:
    RamUsersWizardPage(RamJsonObjectModel *users, bool serverEdit, QWidget *parent = nullptr);

    void reinit();
    virtual void initializePage() override;

private slots:
    void editUser(const QModelIndex &index);

private:
    void setupUi();
    void connectEvents();

    void initializeAssignList();
    void initializeServerList();
    void addUserToModel(const QJsonValue &userVal, RamJsonObjectModel *model);
    QJsonArray getServerUsers();

    RamJsonObjectModel *_users;
    RamJsonObjectModel *_assignList = nullptr;
    DuListEditView *ui_userList;
    bool _editingServer = false;
    bool _initialized = false;
};

#endif // RAMUSERSWIZARDPAGE_H
