#ifndef USEREDITWIDGET_H
#define USEREDITWIDGET_H

#include "ui_usereditwidget.h"
#include "ramses.h"
#include "duqf-widgets/duqffolderselectorwidget.h"

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
    void updateFolderLabel(QString path);
    void userRemoved(RamObject *o);
    void dbiLog(DuQFLog m);

private:
    DuQFFolderSelectorWidget *folderSelector;
    RamUser *_user;
    QMetaObject::Connection _currentUserConnection;
};

#endif // USEREDITWIDGET_H
