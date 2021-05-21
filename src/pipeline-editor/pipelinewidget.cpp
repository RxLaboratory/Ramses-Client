#include "pipelinewidget.h"

#include "mainwindow.h"

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
    stepMenu = new QMenu(this);

    QAction *actionAddStep = new QAction("Create new step", this);
    actionAddStep->setShortcut(QKeySequence("Shift+A"));
    stepMenu->addAction(actionAddStep);
    stepMenu->addSeparator();
    stepMenuSeparator = stepMenu->addSeparator();

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

    for (int i = 0; i < Ramses::instance()->templateSteps()->count(); i++) newTemplateStep( Ramses::instance()->templateSteps()->at(i) );

    titleBar->insertLeft(stepButton);

    // Connections menu
    QMenu *coMenu = new QMenu(this);

    QAction *actionDeleteConnections = new QAction("Remove selected pipes", this);
    actionDeleteConnections->setShortcut(QKeySequence("Alt+X"));
    coMenu->addAction(actionDeleteConnections);

    QToolButton *coButton = new QToolButton(this);
    coButton->setText("Pipe");
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
    connect(actionAddStep, SIGNAL(triggered()), this, SLOT(createStep()));
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
    connect(_nodeScene->connectionManager(), SIGNAL(newConnection(DuQFConnection*)), this, SLOT(stepsConnected(DuQFConnection*)));
    connect(_nodeScene->connectionManager(), SIGNAL(connectionRemoved(DuQFConnection*)), this, SLOT(connectionRemoved(DuQFConnection*)));
    // Ramses connections
    connect(Ramses::instance()->templateSteps(), &RamObjectList::objectAdded, this, &PipelineWidget::newTemplateStep);
    connect(Ramses::instance()->templateSteps(), &RamObjectList::objectRemoved, this, &PipelineWidget::templateStepRemoved);
    connect(Ramses::instance(), &Ramses::currentProjectChanged, this, &PipelineWidget::changeProject);
    connect(Ramses::instance(), &Ramses::loggedIn, this, &PipelineWidget::userChanged);
}

void PipelineWidget::changeProject(RamProject *project)
{
    DBISuspender b;

    QSignalBlocker b1(_nodeScene);
    QSignalBlocker b2(_nodeView);
    QSignalBlocker b3(_nodeScene->connectionManager());

    this->setEnabled(false);

    while (_projectConnections.count() > 0) disconnect( _projectConnections.takeLast() );

    // Clear scene
    _nodeScene->clear();
    _nodeView->reinitTransform();

    if (!project) return;

    // add steps
    foreach(RamStep *step, project->steps()) newStep(step);

    // add pipes
    for ( int i = 0; i < project->pipeline()->count(); i++ ) newPipe( project->pipeline()->at(i) );

    _projectConnections << connect(project, &RamProject::newStep, this, &PipelineWidget::newStep);
    _projectConnections << connect(project->pipeline(), &RamObjectList::objectAdded, this, &PipelineWidget::newPipe);
    _projectConnections << connect(project->pipeline(), &RamObjectList::objectRemoved, this, &PipelineWidget::pipeRemoved);

    // Layout
    _nodeScene->clearSelection();
    //_nodeView->frameSelected();

    this->setEnabled(true);
}

void PipelineWidget::newStep(RamStep *step)
{
    StepNode *stepNode = new StepNode(step);
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

void PipelineWidget::createStep()
{
    RamProject *project = Ramses::instance()->currentProject();
    if (!project) return;

    project->createStep();
}

void PipelineWidget::newTemplateStep(RamObject *obj)
{
    if (!obj) return;
    if (obj->uuid() == "") return;
    QAction *stepAction = new QAction(obj->name());
    stepAction->setData(obj->uuid());
    stepMenu->insertAction(stepMenuSeparator, stepAction);
    connect(stepAction, &QAction::triggered, this, &PipelineWidget::assignStep);
    connect(obj, &RamObject::changed, this, &PipelineWidget::templateStepChanged);

}

void PipelineWidget::templateStepRemoved(RamObject *o)
{
    QList<QAction *> actions = stepMenu->actions();
    for (int i = actions.count() -1; i >= 0; i--)
    {
        if (actions[i]->data().toString() == o->uuid())
        {
            stepMenu->removeAction(actions[i]);
            actions[i]->deleteLater();
        }
    }
}

void PipelineWidget::templateStepChanged()
{
    RamStep *s = (RamStep*)sender();
    QList<QAction *> actions = stepMenu->actions();
    for (int i = actions.count() -1; i >= 0; i--)
    {
        if (actions[i]->data().toString() == s->uuid()) actions[i]->setText(s->name());
    }
}

void PipelineWidget::assignStep()
{
    RamProject *project = Ramses::instance()->currentProject();
    if (!project) return;
    QAction *stepAction = (QAction*)sender();
    RamStep *templateStep = Ramses::instance()->templateStep(stepAction->data().toString());
    if (!templateStep) return;
    project->assignStep(templateStep);
}

void PipelineWidget::newPipe(RamObject *p)
{
    RamPipe *pipe = qobject_cast<RamPipe*>(p);
    if (!pipe) return;

    // Get nodes
    DuQFNode *inputNode = nullptr;
    DuQFNode *outputNode = nullptr;
    foreach(DuQFNode *n, _nodeScene->nodes())
    {
        StepNode *is = (StepNode*)n;
        if (is) if (is->step()->uuid() == pipe->inputStep()->uuid()) inputNode = n;

        StepNode *os = (StepNode*)n;
        if (os) if (os->step()->uuid() == pipe->outputStep()->uuid()) outputNode = n;

        if (inputNode && outputNode) break;
    }

    if (!outputNode) return;
    if (!inputNode) return;

    QSignalBlocker b(_nodeScene->connectionManager());

    // Get or create the node connections
    DuQFConnection *co = _nodeScene->connectNodes(outputNode, inputNode);
    if (!co) return;

    RamFileType *ft = pipe->fileType();
    if (ft) co->connector()->setTitle( ft->shortName() );

    // Create an edit dockwidget
    ObjectDockWidget *dockWidget = new ObjectDockWidget(pipe);
    dockWidget->setTitle("Pipe");
    dockWidget->setIcon(":/icons/connection");
    PipeEditWidget *editWidget = new PipeEditWidget(pipe, dockWidget);
    dockWidget->setWidget(editWidget);
    MainWindow *mw = (MainWindow*)GuiUtils::appMainWindow();
    mw->addObjectDockWidget(dockWidget);
    dockWidget->hide();

    connect(pipe, SIGNAL(changed(RamObject*)), this, SLOT(pipeChanged(RamObject*)));
    connect(co->connector(), SIGNAL(selected(bool)), dockWidget, SLOT(setVisible(bool)));

    _pipeConnections[pipe->uuid()] = co;
}

void PipelineWidget::stepsConnected(DuQFConnection *co)
{
    RamProject *project = Ramses::instance()->currentProject();
    if (!project) return;
    // Get steps
    StepNode *outputNode = (StepNode*)co->outputNode();
    StepNode *inputNode = (StepNode*)co->inputNode();
    if (!outputNode) return;
    if (!inputNode) return;
    RamStep *output = outputNode->step();
    RamStep *input = inputNode->step();
    if (!output) return;
    if (!input) return;

    project->createPipe(output, input);
}

void PipelineWidget::connectionRemoved(DuQFConnection *co)
{
    RamProject *project = Ramses::instance()->currentProject();
    if (!project) return;
    // Get steps
    StepNode *outputNode = (StepNode*)co->outputNode();
    StepNode *inputNode = (StepNode*)co->inputNode();
    if (!outputNode) return;
    if (!inputNode) return;
    RamStep *output = outputNode->step();
    RamStep *input = inputNode->step();
    if (!output) return;
    if (!input) return;

    RamPipe *p = project->pipe(output, input);
    if (p) p->remove();
}

void PipelineWidget::pipeChanged(RamObject *p)
{
    RamPipe *pipe = (RamPipe*)p;

    if (_pipeConnections.contains(pipe->uuid()))
    {
        DuQFConnection *co = _pipeConnections.value(pipe->uuid());
        //update stepnodes
        bool outputOk = false;
        bool inputOk = false;
        foreach(DuQFNode *n, _nodeScene->nodes())
        {
            StepNode *outputNode = (StepNode*)n;
            if (outputNode) if (outputNode->step()->uuid() == pipe->outputStep()->uuid())
            {
                co->setOutput(outputNode->defaultOutputSlot());
                outputOk = true;
            }

            StepNode *inputNode = (StepNode*)n;
            if (inputNode) if (inputNode->step()->uuid() == pipe->inputStep()->uuid())
            {
                co->setInput(inputNode->defaultInputSlot());
                inputOk = true;
            }

            if (inputOk && outputOk) break;
        }

        RamFileType *ft = pipe->fileType();
        if (ft) co->connector()->setTitle(ft->shortName());
        else co->connector()->setTitle("");
    }

}

void PipelineWidget::pipeRemoved(RamObject *p)
{
    if (_pipeConnections.contains(p->uuid()))
    {
        //remove connection
        _pipeConnections.value(p->uuid())->remove();
        //and remove its pointer from the list
        _pipeConnections.remove(p->uuid());
    }
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

