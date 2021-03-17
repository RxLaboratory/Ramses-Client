#include "pipelinewidget.h"

PipelineWidget::PipelineWidget(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);

    // Get the mainwindow to add the titlebar
    QMainWindow *mw = GuiUtils::appMainWindow();
    mw->addToolBarBreak(Qt::TopToolBarArea);

    titleBar = new TitleBar("Pipeline Editor",false, mw);
    titleBar->showReinitButton(false);
    mw->addToolBar(Qt::TopToolBarArea,titleBar);
    titleBar->setFloatable(false);
    titleBar->hide();

    // View menu
    QMenu *viewMenu = new QMenu(this);

    QAction *actionReinitView = new QAction("Center view", this);
    actionReinitView->setShortcut(QKeySequence("Home"));
    viewMenu->addAction(actionReinitView);

    QAction *actionViewAll = new QAction("Center selection", this);
    actionViewAll->setShortcut(QKeySequence("F"));
    viewMenu->addAction(actionViewAll);

    QToolButton *viewButton = new QToolButton(this);
    viewButton->setText("View");
    viewButton->setIcon(QIcon(":/icons/view"));
    viewButton->setIconSize(QSize(16,16));
    viewButton->setObjectName("menuButton");
    viewButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    viewButton->setPopupMode(QToolButton::InstantPopup);
    viewButton->setMenu(viewMenu);

    titleBar->insertLeft(viewButton);

    // Select menu
    QMenu *selectMenu = new QMenu(this);

    QAction *actionSelectAll = new QAction("Select all steps", this);
    actionSelectAll->setShortcut(QKeySequence("A"));
    selectMenu->addAction(actionSelectAll);

    QAction *actionSelectChildren = new QAction("Select children steps", this);
    actionSelectChildren->setShortcut(QKeySequence("Ctrl+A"));
    selectMenu->addAction(actionSelectChildren);

    QAction *actionSelectParents = new QAction("Select parent steps", this);
    actionSelectParents->setShortcut(QKeySequence("Alt+A"));
    selectMenu->addAction(actionSelectParents);

    QToolButton *selectButton = new QToolButton(this);
    selectButton->setText("Select");
    selectButton->setIcon(QIcon(":/icons/select-menu"));
    selectButton->setIconSize(QSize(16,16));
    selectButton->setObjectName("menuButton");
    selectButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    selectButton->setPopupMode(QToolButton::InstantPopup);
    selectButton->setMenu(selectMenu);

    titleBar->insertLeft(selectButton);

    // Layout menu
    QMenu *layMenu = new QMenu(this);

    QAction *actionLayoutAll = new QAction("Layout all steps", this);
    actionLayoutAll->setShortcut(QKeySequence("Shift+L"));
    layMenu->addAction(actionLayoutAll);

    QAction *actionLayoutSelected = new QAction("Layout selected steps", this);
    actionLayoutSelected->setShortcut(QKeySequence("Alt+L"));
    layMenu->addAction(actionLayoutSelected);

    QToolButton *layButton = new QToolButton(this);
    layButton->setText("Layout");
    layButton->setIcon(QIcon(":/icons/steps-menu"));
    layButton->setIconSize(QSize(16,16));
    layButton->setObjectName("menuButton");
    layButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    layButton->setPopupMode(QToolButton::InstantPopup);
    layButton->setMenu(layMenu);

    titleBar->insertLeft(layButton);

    // Step menu
    QMenu *stepMenu = new QMenu(this);

    QAction *actionAddStep = new QAction("New step", this);
    actionAddStep->setShortcut(QKeySequence("Shift+A"));
    stepMenu->addAction(actionAddStep);

    QAction *actionDeleteStep = new QAction("Remove selected steps", this);
    actionDeleteStep->setShortcut(QKeySequence("Shift+X"));
    stepMenu->addAction(actionDeleteStep);

    QAction *actionDeleteSelection = new QAction("Delete selection", this);
    actionDeleteSelection->setShortcut(QKeySequence("Delete"));
    stepMenu->addAction(actionDeleteSelection);

    QToolButton *stepButton = new QToolButton(this);
    stepButton->setText("Step");
    stepButton->setIcon(QIcon(":/icons/step"));
    stepButton->setIconSize(QSize(16,16));
    stepButton->setObjectName("menuButton");
    stepButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    stepButton->setPopupMode(QToolButton::InstantPopup);
    stepButton->setMenu(stepMenu);

    titleBar->insertLeft(stepButton);

    // Connections menu
    QMenu *coMenu = new QMenu(this);

    QAction *actionDeleteConnections = new QAction("Remove selected connections", this);
    actionDeleteConnections->setShortcut(QKeySequence("Alt+X"));
    coMenu->addAction(actionDeleteConnections);

    QToolButton *coButton = new QToolButton(this);
    coButton->setText("Connection");
    coButton->setIcon(QIcon(":/icons/connection"));
    coButton->setIconSize(QSize(16,16));
    coButton->setObjectName("menuButton");
    coButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    coButton->setPopupMode(QToolButton::InstantPopup);
    coButton->setMenu(coMenu);

    titleBar->insertLeft(coButton);

    // Right buttons

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

    gridSizeBox = new DuQFSpinBox(this);
    gridSizeBox->setMinimum(10);
    gridSizeBox->setMaximum(100);
    gridSizeBox->setMaximumWidth(100);
    gridSizeBox->setValue(20);
    gridSizeBox->setPrefix("Grid size: ");
    titleBar->insertRight(gridSizeBox);

    snapButton = new QCheckBox("Snap to grid");
    titleBar->insertRight(snapButton);

    _nodeView = new DuQFNodeView(this);
    _nodeScene = _nodeView->nodeScene();
    mainLayout->addWidget(_nodeView);

    userSettings = new QSettings(QSettings::IniFormat, QSettings::UserScope, STR_COMPANYNAME, STR_INTERNALNAME);
    userSettings->beginGroup("nodeView");

    // Connections
    connect(titleBar, &TitleBar::closeRequested, this, &PipelineWidget::closeRequested);
    connect(viewAllButton, SIGNAL(clicked()), _nodeView, SLOT(reinitTransform()));
    connect(actionReinitView, SIGNAL(triggered()), _nodeView, SLOT(reinitTransform()));
    connect(viewSelectedButton, SIGNAL(clicked()), _nodeView, SLOT(frameSelected()));
    connect(actionViewAll, SIGNAL(triggered()), _nodeView, SLOT(frameSelected()));
    connect(zoomBox, SIGNAL(valueChanged(int)), _nodeView, SLOT(setZoom(int)));
    connect(_nodeView, SIGNAL(zoomed(int)), zoomBox, SLOT(setValue(int)));
    connect(actionAddStep, SIGNAL(triggered()), _nodeScene, SLOT(addNode()));
    connect(actionAddStep, SIGNAL(triggered()), _nodeView, SLOT(reinitTransform()));
    connect(actionDeleteStep, SIGNAL(triggered()), _nodeScene, SLOT(removeSelectedNodes()));
    connect(actionDeleteConnections, SIGNAL(triggered()), _nodeScene, SLOT(removeSelectedConnections()));
    connect(actionDeleteSelection, SIGNAL(triggered()), _nodeScene, SLOT(removeSelection()));
    connect(snapButton, SIGNAL(clicked(bool)), this, SLOT(setSnapEnabled(bool)));
    connect(gridSizeBox, SIGNAL(valueChanged(int)), this, SLOT(setGridSize(int)));
    connect(actionLayoutAll, SIGNAL(triggered()), _nodeScene, SLOT(autoLayoutAll()));
    connect(actionLayoutAll, SIGNAL(triggered()), _nodeView, SLOT(frameSelected()));
    connect(actionLayoutSelected, SIGNAL(triggered()), _nodeScene, SLOT(autoLayoutSelectedNodes()));
    connect(actionSelectAll, SIGNAL(triggered()), _nodeScene, SLOT(selectAllNodes()));
    connect(actionSelectChildren, SIGNAL(triggered()), _nodeScene, SLOT(selectChildNodes()));
    connect(actionSelectParents, SIGNAL(triggered()), _nodeScene, SLOT(selectParentNodes()));
    // Ramses connections
    connect(Ramses::instance(), &Ramses::projectChanged, this, &PipelineWidget::changeProject);
    connect(Ramses::instance(), &Ramses::loggedIn, this, &PipelineWidget::userChanged);
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
    //_nodeView->frameSelected();

    this->setEnabled(true);
}

void PipelineWidget::newStep(RamStep *step)
{
    ObjectNode *stepNode = new ObjectNode(step);
    _nodeScene->addNode( stepNode );

    // Reset position
    userSettings->beginGroup("nodeLocations");
    QPointF pos = userSettings->value(step->uuid(), QPointF(0.0,0.0)).toPointF();
    if (pos.x() != 0.0 && pos.y() != 0.0) stepNode->setPos( pos );
    userSettings->endGroup();

    connect(stepNode, &DuQFNode::moved, this, &PipelineWidget::nodeMoved);
}

void PipelineWidget::nodeMoved(QPointF pos)
{
    ObjectNode *node = (ObjectNode*)sender();
    RamObject *step = node->ramObject();

    userSettings->beginGroup("nodeLocations");
    userSettings->setValue(step->uuid(), pos);
    userSettings->endGroup();
}

void PipelineWidget::setSnapEnabled(bool enabled)
{
    QSignalBlocker b(snapButton);

    snapButton->setChecked(enabled);

    userSettings->setValue("snapToGrid", enabled);
    _nodeView->grid()->setSnapEnabled(enabled);
}

void PipelineWidget::setGridSize(int size)
{
    QSignalBlocker b(gridSizeBox);

    gridSizeBox->setValue(size);

    userSettings->setValue("gridSize", size);
    _nodeView->grid()->setSize(size);
    _nodeView->update();
}

void PipelineWidget::userChanged(RamUser *u)
{
    Q_UNUSED(u);

    userSettings->endGroup();
    delete userSettings;
    userSettings = new QSettings(Ramses::instance()->currentUserSettingsFile(), QSettings::IniFormat, this);
    userSettings->beginGroup("nodeView");

    setSnapEnabled(userSettings->value("snapToGrid", false).toBool());
    setGridSize(userSettings->value("gridSize", 20).toInt());
}

void PipelineWidget::showEvent(QShowEvent *event)
{
    if (!event->spontaneous()) titleBar->show();
    QWidget::showEvent(event);
}

void PipelineWidget::hideEvent(QHideEvent *event)
{
    if (!event->spontaneous())
    {
        _nodeScene->clearSelection();
        _nodeScene->clearFocus();
        titleBar->hide();
    }
    QWidget::hideEvent(event);
}

