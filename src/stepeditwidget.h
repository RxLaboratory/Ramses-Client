#ifndef STEPEDITWIDGET_H
#define STEPEDITWIDGET_H

#include <QMenu>

#include "ui_stepeditwidget.h"
#include "ramses.h"
#include "duqf-widgets/duqffolderdisplaywidget.h"

class StepEditWidget : public QWidget, private Ui::StepEditWidget
{
    Q_OBJECT

public:
    explicit StepEditWidget(QWidget *parent = nullptr);

    RamStep *step() const;
    void setStep(RamStep *step);

private slots:
    void update();
    void revert();
    bool checkInput();
    void stepDestroyed(QObject *o);
    // Users
    void newUser(RamUser *user);
    void assignUser();
    void unassignUser();
    void userAssigned(RamUser *user);
    void userUnassigned(QString uuid);
    void userChanged();
    void userDestroyed(QObject *o);
    // Applications
    void newApplication(RamApplication *app);
    void assignApplication();
    void unassignApplication();
    void applicationAssigned(RamApplication *app);
    void applicationUnassigned(QString uuid);
    void applicationChanged();
    void applicationRemoved(RamObject *o);
    // Misc
    void dbiLog(DuQFLog m);

private:
    RamStep *_step;
    QMenu *assignUserMenu;
    QMenu *assignAppMenu;
    DuQFFolderDisplayWidget *folderWidget;
    QList<QMetaObject::Connection> _stepConnections;
};

#endif // STEPEDITWIDGET_H
