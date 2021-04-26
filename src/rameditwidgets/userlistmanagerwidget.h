#ifndef USERLISTMANAGERWIDGET_H
#define USERLISTMANAGERWIDGET_H

#include <QWidget>
#include <QHBoxLayout>
#include <QSplitter>

#include "ramses.h"
#include "objectlisteditwidget.h"
#include "usereditwidget.h"

class UserListManagerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit UserListManagerWidget(QWidget *parent = nullptr);

signals:

private slots:
    void editNewUser(RamObject *u);

private:
    void setupUi();
    void connectEvents();

    ObjectListEditWidget *m_listEditWidget;
    UserEditWidget *m_editWidget;
};

#endif // USERLISTMANAGERWIDGET_H
