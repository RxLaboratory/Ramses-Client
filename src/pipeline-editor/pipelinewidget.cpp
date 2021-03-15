#include "pipelinewidget.h"

PipelineWidget::PipelineWidget(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);

    titleBar = new TitleBar("Pipeline Editor",this);
    titleBar->showReinitButton(false);

    titleBar->insertLeft(actionAddNode);
    titleBar->insertLeft(actionDeleteNodes);
    titleBar->insertLeft(actionDeleteConnections);

    QToolButton *viewAllButton = new QToolButton();
    viewAllButton->setIcon(QIcon(":/icons/view-all"));
    titleBar->insertRight(viewAllButton);
    QToolButton *viewSelectedButton = new QToolButton();
    viewSelectedButton->setIcon(QIcon(":/icons/view-selected"));
    titleBar->insertRight(viewSelectedButton);

    DuQFSpinBox *zoomBox = new DuQFSpinBox(this);
    zoomBox->setMinimum(25);
    zoomBox->setMaximum(400);
    zoomBox->setSuffix("%");
    zoomBox->setPrefix("Zoom: ");
    zoomBox->setMaximumWidth(100);
    zoomBox->setValue(100);
    titleBar->insertRight(zoomBox);

    DuQFSpinBox *gridSizeBox = new DuQFSpinBox(this);
    gridSizeBox->setMinimum(10);
    gridSizeBox->setMaximum(100);
    gridSizeBox->setMaximumWidth(100);
    gridSizeBox->setValue(20);
    gridSizeBox->setPrefix("Grid size: ");
    titleBar->insertRight(gridSizeBox);

    QCheckBox *snapButton = new QCheckBox("Snap to grid");
    titleBar->insertRight(snapButton);

    mainLayout->addWidget(titleBar);

    DuQFNodeView *view = new DuQFNodeView(this);
    DuQFNodeScene *scene = view->nodeScene();
    mainLayout->addWidget(view);

    // Connections
    connect(titleBar, &TitleBar::closeRequested, this, &PipelineWidget::closeRequested);
    connect(viewAllButton, SIGNAL(clicked()), view, SLOT(reinitTransform()));
    connect(viewSelectedButton, SIGNAL(clicked()), view, SLOT(frameSelected()));
    connect(zoomBox, SIGNAL(valueChanged(int)), view, SLOT(setZoom(int)));
    connect(view, SIGNAL(zoomed(int)), zoomBox, SLOT(setValue(int)));
    connect(actionAddNode, SIGNAL(triggered()), scene, SLOT(addNode()));
    connect(actionDeleteNodes, SIGNAL(triggered()), scene, SLOT(removeSelectedNodes()));
    connect(actionDeleteConnections, SIGNAL(triggered()), scene, SLOT(removeSelectedConnections()));
    connect(snapButton, SIGNAL(clicked(bool)), &view->grid(), SLOT(setSnapEnabled(bool)));
    connect(gridSizeBox, SIGNAL(valueChanged(int)), &view->grid(), SLOT(setGridSize(int)));
}
