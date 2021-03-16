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

#include "ramobjectnode.h"
#include "ramses.h"

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

private:
    QSettings *userSettings;
    TitleBar *titleBar;
    DuQFSpinBox *gridSizeBox;
    QCheckBox *snapButton;
    DuQFNodeScene *_nodeScene;
    DuQFNodeView *_nodeView;
    QList<QMetaObject::Connection> _projectConnections;
};

#endif // PIPELINEWIDGET_H
