#ifndef RAMOBJECTWIDGET_H
#define RAMOBJECTWIDGET_H

#include <QWidget>
#include <QDockWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QToolButton>

#include "ramses.h"
#include "objecteditwidget.h"
#include "objectdockwidget.h"

class RamObjectWidget : public QWidget
{
    Q_OBJECT
public:
    explicit RamObjectWidget(RamObject *obj, QWidget *parent = nullptr);
    ~RamObjectWidget();
    RamObject *ramObject() const;
    void setObject(RamObject *o);

    void setUserEditRole(RamUser::UserRole role);

    void setTitle(QString t);

    ObjectDockWidget *dockEditWidget() const;

public slots:
    void edit();

protected:
    void setEditWidget(ObjectEditWidget *w);

private slots:
    void objectRemoved();
    void userChanged();


private:
    void setupUi();
    QToolButton *editButton;
    QLabel *title;
    QLabel *icon;

    bool _hasEditWidget = false;
    RamObject *_object;
    ObjectDockWidget *_dockEditWidget;
    RamUser::UserRole _editRole = RamUser::Admin;
};

#endif // RAMOBJECTWIDGET_H
