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

    QToolButton *autoLayoutButton = new QToolButton(this);
    autoLayoutButton->setText("Auto Layout");
    titleBar->insertRight(autoLayoutButton);
    QToolButton *viewAllButton = new QToolButton(this);
    viewAllButton->setIcon(QIcon(":/icons/view-all"));
    titleBar->insertRight(viewAllButton);
    QToolButton *viewSelectedButton = new QToolButton(this);
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

    _nodeView = new DuQFNodeView(this);
    _nodeScene = _nodeView->nodeScene();
    mainLayout->addWidget(_nodeView);

    // Connections
    connect(titleBar, &TitleBar::closeRequested, this, &PipelineWidget::closeRequested);
    connect(viewAllButton, SIGNAL(clicked()), _nodeView, SLOT(reinitTransform()));
    connect(viewSelectedButton, SIGNAL(clicked()), _nodeView, SLOT(frameSelected()));
    connect(zoomBox, SIGNAL(valueChanged(int)), _nodeView, SLOT(setZoom(int)));
    connect(_nodeView, SIGNAL(zoomed(int)), zoomBox, SLOT(setValue(int)));
    connect(actionAddNode, SIGNAL(triggered()), _nodeScene, SLOT(addNode()));
    connect(actionAddNode, SIGNAL(triggered()), _nodeView, SLOT(reinitTransform()));
    connect(actionDeleteNodes, SIGNAL(triggered()), _nodeScene, SLOT(removeSelectedNodes()));
    connect(actionDeleteConnections, SIGNAL(triggered()), _nodeScene, SLOT(removeSelectedConnections()));
    connect(snapButton, SIGNAL(clicked(bool)), &_nodeView->grid(), SLOT(setSnapEnabled(bool)));
    connect(gridSizeBox, SIGNAL(valueChanged(int)), &_nodeView->grid(), SLOT(setGridSize(int)));
    connect(autoLayoutButton, SIGNAL(clicked()), _nodeScene, SLOT(autoLayoutNodes()));
    connect(autoLayoutButton, SIGNAL(clicked()), _nodeView, SLOT(frameSelected()));
    // Ramses connections
    connect(Ramses::instance(), &Ramses::projectChanged, this, &PipelineWidget::changeProject);
}

void PipelineWidget::changeProject(RamProject *project)
{
    this->setEnabled(false);

    while (_projectConnections.count() > 0) disconnect( _projectConnections.takeLast() );

    // Clear scene
    _nodeScene->clear();
    _nodeView->reinitTransform();

    if (!project) return;

    // add steps
    foreach(RamStep *step, project->steps()) newStep(step);

    _projectConnections << connect(project, &RamProject::newStep, this, &PipelineWidget::newStep);

    // Layout
    _nodeScene->clearSelection();
    _nodeScene->autoLayoutNodes();
    _nodeView->frameSelected();

    this->setEnabled(true);
}

void PipelineWidget::newStep(RamStep *step)
{
    _nodeScene->addNode( new RamObjectNode(step) );
}
