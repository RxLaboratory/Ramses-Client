#ifndef PIPELINEWIDGET_H
#define PIPELINEWIDGET_H

#include <QCheckBox>
#include <QMenu>
#include <QSettings>

#include "ui_pipelinewidget.h"
#include "duqf-nodeview/duqfnodeview.h"
#include "duqf-nodeview/duqfnodescene.h"
#include "duqf-widgets/titlebar.h"
#include "duqf-widgets/duqfspinbox.h"
#include "duqf-app/app-version.h"
#include "duqf-utils/guiutils.h"

#include "stepnode.h"
#include "ramses.h"
#include "pipeeditwidget.h"
#include "objectdockwidget.h"

class PipelineWidget : public QWidget, private Ui::PipelineWidget
{
    Q_OBJECT

public:
    explicit PipelineWidget(QWidget *parent = nullptr);
signals:
    void closeRequested();
private slots:
    void changeProject(RamProject *project);
    void newStep(RamStep *step);
    void nodeMoved(QPointF pos);
    void setSnapEnabled(bool enabled);
    void setGridSize(int size);
    void userChanged(RamUser *u);
    void createStep();
    void newTemplateStep(RamObject *obj);
    void templateStepRemoved(RamObject *o);
    void templateStepChanged();
    void assignStep();
    void newPipe(RamObject *p);
    void stepsConnected(DuQFConnection *co);
    void connectionRemoved(DuQFConnection *co);
    void pipeChanged(RamObject *p);
    void pipeRemoved(RamObject *p);
protected:
    void showEvent(QShowEvent *event) Q_DECL_OVERRIDE;
    void hideEvent(QHideEvent *event) Q_DECL_OVERRIDE;
private:
    QSettings *userSettings;
    TitleBar *titleBar;
    DuQFSpinBox *gridSizeBox;
    QCheckBox *snapButton;
    DuQFNodeScene *_nodeScene;
    DuQFNodeView *_nodeView;
    QMenu *stepMenu;
    QAction *stepMenuSeparator;
    QList<QMetaObject::Connection> _projectConnections;
    QMap<QString,DuQFConnection*> _pipeConnections;
};

#endif // PIPELINEWIDGET_H
