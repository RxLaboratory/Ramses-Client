#ifndef USEREDITWIDGET_H
#define USEREDITWIDGET_H

#include "ui_usereditwidget.h"
#include "ramses.h"

class UserEditWidget : public QWidget, private Ui::UserEditWidget
{
    Q_OBJECT

public:
    explicit UserEditWidget(QWidget *parent = nullptr);

    RamUser *user() const;
    void setUser(RamUser *user);

private slots:
    void update();
    void revert();
    bool checkInput();
    void userDestroyed(QObject *o);
    void dbiLog(QString m, LogUtils::LogType t);

private:
    RamUser *_user;
    QMetaObject::Connection _currentUserConnection;
};

#endif // USEREDITWIDGET_H
