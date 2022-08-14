#ifndef USERLISTMANAGERWIDGET_H
#define USERLISTMANAGERWIDGET_H

#include "objectlistmanagerwidget.h"
#include "ramuser.h"

class UserListManagerWidget : public ObjectListManagerWidget<RamUser*, int>
{
    Q_OBJECT
public:
    explicit UserListManagerWidget(QWidget *parent = nullptr);

protected slots:
    RamUser *createObject() override;
};

#endif // USERLISTMANAGERWIDGET_H
