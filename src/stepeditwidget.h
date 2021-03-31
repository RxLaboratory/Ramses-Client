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
#include "duqf-widgets/duqffolderdisplaywidget.h"

class StepEditWidget : public ObjectEditWidget
{
    Q_OBJECT
public:
    StepEditWidget(QWidget *parent = nullptr);
    StepEditWidget(RamStep *s, QWidget *parent = nullptr);

    void setStep(RamStep *s);

private slots:
    void update() Q_DECL_OVERRIDE;
    // Users
    void newUser(RamUser *user);
    void assignUser();
    void unassignUser();
    void userAssigned(RamUser *user);
    void userUnassigned(QString uuid);
    void userChanged();
    void userRemoved(RamObject *o);
    // Applications
    void newApplication(RamApplication *app);
    void assignApplication();
    void unassignApplication();
    void applicationAssigned(RamApplication *app);
    void applicationUnassigned(QString uuid);
    void applicationChanged();
    void applicationRemoved(RamObject *o);

private:
    void setupUi();
    void populateMenus();
    void connectEvents();

    QComboBox *typeBox;
    DuQFFolderDisplayWidget *folderWidget;
    QToolButton *assignUserButton;
    QToolButton *removeUserButton;
    QListWidget *usersList;
    QToolButton *assignApplicationButton;
    QToolButton *removeApplicationButton;
    QListWidget *applicationList;
    QMenu *assignUserMenu;
    QMenu *assignAppMenu;
};

#endif // STEPEDITWIDGET_H
