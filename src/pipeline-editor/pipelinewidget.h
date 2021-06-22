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

#include "processmanager.h"
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
    void setProject(RamProject *project);
    void newStep(RamObject *obj);
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
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;
private:
    void changeProject();

    TitleBar *ui_titleBar;
    DuQFSpinBox *ui_gridSizeBox;
    QCheckBox *ui_snapButton;
    DuQFNodeScene *m_nodeScene;
    DuQFNodeView *ui_nodeView;
    QMenu *ui_stepMenu;
    QAction *ui_stepMenuSeparator;

    QList<QMetaObject::Connection> m_projectConnections;
    QMap<QString, QList<QMetaObject::Connection>> m_pipeObjectConnections;
    QMap<QString,DuQFConnection*> m_pipeConnections;

    RamProject *m_project = nullptr;
    bool m_projectChanged = false;
};

#endif // PIPELINEWIDGET_H
