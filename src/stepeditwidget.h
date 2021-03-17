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
    void newUser(RamUser *user);
    void assignUser();
    void unassignUser();
    void userAssigned(RamUser *user);
    void userRemoved(QString uuid);
    void userChanged();
    void userDestroyed(QObject *o);
    void dbiLog(DuQFLog m);

private:
    RamStep *_step;
    QMenu *assignMenu;
    DuQFFolderDisplayWidget *folderWidget;
    QList<QMetaObject::Connection> _stepConnections;
};

#endif // STEPEDITWIDGET_H
