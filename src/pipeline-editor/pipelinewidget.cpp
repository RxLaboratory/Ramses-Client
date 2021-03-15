#include "pipelinewidget.h"

PipelineWidget::PipelineWidget(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);

    titleBar = new TitleBar("Pipeline Editor",this);

    titleBar->insertLeft(actionAddNode);
    titleBar->insertLeft(actionDeleteNodes);
    titleBar->insertLeft(actionDeleteConnections);

    titleBar->insertRight(actionViewAll);
    titleBar->insertRight(actionViewSelected);
    titleBar->showReinitButton(false);

    DuQFSpinBox *zoomBox = new DuQFSpinBox(this);
    zoomBox->setMinimum(25);
    zoomBox->setMaximum(400);
    zoomBox->setSuffix("%");
    zoomBox->setMaximumWidth(100);
    zoomBox->setValue(100);
    titleBar->insertRight(zoomBox);

    mainLayout->addWidget(titleBar);

    DuQFNodeView *view = new DuQFNodeView(this);
    DuQFNodeScene *scene = view->nodeScene();
    mainLayout->addWidget(view);

    // Connections
    connect(titleBar, &TitleBar::closeRequested, this, &PipelineWidget::closeRequested);
    connect(actionViewAll, SIGNAL(triggered()), view, SLOT(reinitTransform()));
    connect(actionViewSelected, SIGNAL(triggered()), view, SLOT(frameSelected()));
    connect(zoomBox, SIGNAL(valueChanged(int)), view, SLOT(setZoom(int)));
    connect(view, SIGNAL(zoomed(int)), zoomBox, SLOT(setValue(int)));
    connect(actionAddNode, SIGNAL(triggered()), scene, SLOT(addNode()));
    connect(actionDeleteNodes, SIGNAL(triggered()), scene, SLOT(removeSelectedNodes()));
    connect(actionDeleteConnections, SIGNAL(triggered()), scene, SLOT(removeSelectedConnections()));
}
