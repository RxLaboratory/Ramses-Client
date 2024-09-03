#ifndef PIPELINEWIDGET_H
#define PIPELINEWIDGET_H

#include <QCheckBox>
#include <QSettings>

#include "ui_pipelinewidget.h"

#include "duwidgets/dumenu.h"
#include "ramproject.h"
#include "ramuser.h"
#include "dunodeview/duqfnodeview.h"
#include "dunodeview/duqfnodescene.h"
#include "duwidgets/duqftitlebar.h"
#include "duwidgets/duqfspinbox.h"

class PipelineWidget : public QWidget, private Ui::PipelineWidget
{
    Q_OBJECT

public:
    explicit PipelineWidget(QWidget *parent = nullptr);
signals:
    void closeRequested();
private slots:
    void nodeMoved(QPointF pos);
    void setSnapEnabled(bool enabled);
    void setGridSize(int size);

    void userChanged(RamUser *u);
    void setProject(RamProject *project);

    void createStep();

    void templateStepInserted(const QModelIndex &parent, int first, int last);
    void newTemplateStep(RamObject *obj);
    void templateStepRemoved(const QModelIndex &parent, int first, int last);
    void templateStepChanged();
    void assignStep();

    void newStep(RamObject *obj);
    void newStep(const QModelIndex &parent, int first, int last);

    void newPipe(RamObject *p);
    void newPipe(const QModelIndex &parent, int first, int last);
    void pipeRemoved(const QModelIndex &parent, int first, int last);
    void pipeChanged(RamObject *p);

    void stepsConnected(DuQFConnection *co);
    void connectionRemoved(DuQFConnection *co);

    void loadProjectLayout();
    void saveProjectLayout();

protected:
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;

private slots:
    void resetProject();

private:
    void changeProject();

    DuQFTitleBar *ui_titleBar;
    DuQFSpinBox *ui_gridSizeBox;
    DuQFSpinBox *ui_curvatureBox;
    QToolButton *ui_snapButton;
    DuQFNodeScene *m_nodeScene;
    DuQFNodeView *ui_nodeView;
    DuMenu *ui_stepMenu;
    QAction *ui_stepMenuSeparator;

    QMap<QString, QList<QMetaObject::Connection>> m_pipeObjectConnections;
    QMap<QString,DuQFConnection*> m_pipeConnections;

    RamProject *m_project = nullptr;
    bool m_projectChanged = false;
    bool init = true;
};

#endif // PIPELINEWIDGET_H
