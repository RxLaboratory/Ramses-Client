#ifndef USERPROFILEPAGE_H
#define USERPROFILEPAGE_H

#include "ui_userprofilepage.h"

#include <QErrorMessage>
#include <QMessageBox>

#include "ramses.h"

class UserProfilePage : public QWidget, private Ui::UserProfilePage
{
    Q_OBJECT

public:
    explicit UserProfilePage(QWidget *parent = nullptr);

private slots:
    void dbiLog(QString m, LogUtils::LogType t);
    void loggedIn(RamUser *user);
    void loggedOut();
    void update();

    bool checkInput();

private:
    Ramses *_ramses;


};

#endif // USERPROFILEPAGE_H
