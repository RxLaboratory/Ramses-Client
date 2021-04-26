#ifndef RAMOBJECTWIDGET_H
#define RAMOBJECTWIDGET_H

#include <QWidget>
#include <QDockWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QToolButton>
#include <QMessageBox>

#include "ramses.h"
#include "objecteditwidget.h"
#include "objectdockwidget.h"
#include "ramobjectlist.h"

/**
 * @brief The RamObjectWidget class is the base class for all small widgets used to show objects anywhere in the UI.
 * It can display a small "edit" button whcih will show an editwidget in the MainWindow Dock
 */
class RamObjectWidget : public QWidget
{
    Q_OBJECT
public:
    explicit RamObjectWidget(RamObject *obj, QWidget *parent = nullptr);
    ~RamObjectWidget();
    RamObject *ramObject() const;
    void setObject(RamObject *o);

    void disableEdit();
    void setEditable(bool editable = true);

    void setUserEditRole(RamUser::UserRole role);
    RamObjectList editUsers() const;

    void setTitle(QString t);

    void setIcon(QString i);

    ObjectDockWidget *dockEditWidget() const;

    bool selected() const;
    void setSelected(bool selected);
    void select();

public slots:
    void edit();

protected:
    void setEditWidget(ObjectEditWidget *w);
    QVBoxLayout *layout;

private slots:
    void objectRemoved();
    void userChanged();
    void remove();

private:
    void setupUi();
    void connectEvents();
    QToolButton *editButton;
    QToolButton *removeButton;
    QLabel *title;
    QLabel *icon;

    bool _selected = false;
    bool _editable = true;

    RamObjectList _editUsers;

    bool _hasEditWidget = false;
    RamObject *_object;
    ObjectDockWidget *_dockEditWidget;
    RamUser::UserRole _editRole = RamUser::Admin;
};

#endif // RAMOBJECTWIDGET_H
