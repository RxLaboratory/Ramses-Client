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
#include "simpleobjectlist.h"
#include "duqf-widgets/duqffolderdisplaywidget.h"

class StepEditWidget : public ObjectEditWidget
{
    Q_OBJECT
public:
    StepEditWidget(QWidget *parent = nullptr);
    StepEditWidget(RamStep *s, QWidget *parent = nullptr);

    RamStep *step() const;
    void setStep(RamStep *s);

protected slots:
    void update() Q_DECL_OVERRIDE;

private slots:
    void stepChanged(RamObject *o);
    // Users
    void newUser(RamUser *user);
    void assignUser();
    void unassignUser(RamObject *o);
    void userAssigned(RamUser *user);
    void userUnassigned(QString uuid);
    void userChanged();
    void userRemoved(RamObject *user);

    // Applications
    void newApplication(RamApplication *app);
    void assignApplication();
    void unassignApplication(RamObject *o);
    void applicationAssigned(RamApplication *app);
    void applicationUnassigned(QString uuid);
    void applicationChanged();
    void applicationRemoved(RamObject *o);

private:
    void setupUi();
    void populateMenus();
    void connectEvents();

    RamStep *_step;

    QComboBox *typeBox;
    DuQFFolderDisplayWidget *folderWidget;
    SimpleObjectList *usersList;
    SimpleObjectList *applicationList;
    QMenu *assignUserMenu;
    QMenu *assignAppMenu;
};

#endif // STEPEDITWIDGET_H
