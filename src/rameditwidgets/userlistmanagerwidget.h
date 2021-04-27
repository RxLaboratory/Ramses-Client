#ifndef USERLISTMANAGERWIDGET_H
#define USERLISTMANAGERWIDGET_H

#include <QWidget>
#include <QHBoxLayout>
#include <QSplitter>

#include "ramses.h"
#include "objectlistmanagerwidget.h"
#include "usereditwidget.h"

class UserListManagerWidget : public ObjectListManagerWidget
{
    Q_OBJECT
public:
    explicit UserListManagerWidget(QWidget *parent = nullptr);

protected slots:
    void createObject() Q_DECL_OVERRIDE;
};

#endif // USERLISTMANAGERWIDGET_H
