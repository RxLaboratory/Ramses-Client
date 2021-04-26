#ifndef USERPROFILEPAGE_H
#define USERPROFILEPAGE_H

#include "ui_userprofilepage.h"

#include <QErrorMessage>
#include <QMessageBox>

#include "ramses.h"
#include "usereditwidget.h"

class UserProfilePage : public QWidget, private Ui::UserProfilePage
{
    Q_OBJECT

public:
    explicit UserProfilePage(QWidget *parent = nullptr);

private slots:
    void loggedIn(RamUser *user);
    void loggedOut();
private:
    void setupUi();
    UserEditWidget *profileWidget;

};

#endif // USERPROFILEPAGE_H
