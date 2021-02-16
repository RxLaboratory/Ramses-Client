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
    bool checkInput();
    void dbiLog(QString m, LogUtils::LogType t);

private:
    RamUser *_user;
};

#endif // USEREDITWIDGET_H
