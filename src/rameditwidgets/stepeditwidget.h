#ifndef STEPEDITWIDGET_H
#define STEPEDITWIDGET_H

#include <QComboBox>
#include <QSplitter>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QToolButton>
#include <QListWidget>
#include <QMenu>

#include "objecteditwidget.h"
#include "ramses.h"
#include "objectlisteditwidget.h"
#include "duqf-widgets/duqffolderdisplaywidget.h"

class StepEditWidget : public ObjectEditWidget
{
    Q_OBJECT
public:
    StepEditWidget(QWidget *parent = nullptr);
    StepEditWidget(RamStep *s, QWidget *parent = nullptr);

    RamStep *step() const;

public slots:
    void setObject(RamObject *obj) Q_DECL_OVERRIDE;

protected slots:
    void update() Q_DECL_OVERRIDE;

private slots:
    // Users
    void newUser(RamObject *user);
    void assignUser();
    void userAssigned(RamObject *user);
    void userUnassigned(RamObject *o);
    void userChanged(RamObject *o);
    void userRemoved(RamObject *user);

    // Applications
    void newApplication(RamObject *app);
    void assignApplication();
    void applicationAssigned(RamObject *o);
    void applicationUnassigned(RamObject *app);
    void applicationChanged(RamObject *o);
    void applicationRemoved(RamObject *o);

private:
    void setupUi();
    void populateMenus();
    void connectEvents();

    RamStep *_step;

    QComboBox *typeBox;
    DuQFFolderDisplayWidget *folderWidget;
    ObjectListEditWidget *usersList;
    ObjectListEditWidget *applicationList;
    QMenu *assignUserMenu;
    QMenu *assignAppMenu;

    QMap<QString, QList<QMetaObject::Connection>> m_userConnections;
    QMap<QString, QList<QMetaObject::Connection>> m_applicationConnections;
};

#endif // STEPEDITWIDGET_H
