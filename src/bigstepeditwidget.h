#ifndef BIGSTEPEDITWIDGET_H
#define BIGSTEPEDITWIDGET_H

#include <QMenu>

#include "ui_stepeditwidget.h"
#include "ramses.h"
#include "duqf-widgets/duqffolderdisplaywidget.h"

class BigStepEditWidget : public QWidget, private Ui::StepEditWidget
{
    Q_OBJECT

public:
    explicit BigStepEditWidget(QWidget *parent = nullptr);

    RamStep *step() const;
    void setStep(RamStep *step);

private slots:
    void update();
    void revert();
    bool checkInput();
    void stepRemoved(RamObject *o);
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
