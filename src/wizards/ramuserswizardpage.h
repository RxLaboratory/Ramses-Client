#ifndef RAMUSERSWIZARDPAGE_H
#define RAMUSERSWIZARDPAGE_H

#include <QWizardPage>

#include "duwidgets/dulistmodeledit.h"
#include "ramjsonobjectmodel.h"

class RamUsersWizardPage : public QWizardPage
{
    Q_OBJECT
public:
    RamUsersWizardPage(RamJsonObjectModel *users, QWidget *parent = nullptr);

    virtual void initializePage() override;

private slots:
    void editUser(const QModelIndex &index);

private:
    void setupUi();
    void connectEvents();

    RamJsonObjectModel *_users;
    DuListModelEdit *ui_userList;
};

#endif // RAMUSERSWIZARDPAGE_H
