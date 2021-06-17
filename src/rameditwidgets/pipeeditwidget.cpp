#include "pipeeditwidget.h"


PipeEditWidget::PipeEditWidget(RamPipe *pipe, QWidget *parent) : ObjectEditWidget(pipe, parent)
{
    setupUi();
    populateMenus(Ramses::instance()->currentProject());
    connectEvents();

    setObject(pipe);
}

void PipeEditWidget::setObject(RamObject *pipeObj)
{
    RamPipe *pipe = qobject_cast<RamPipe*>( pipeObj );
    this->setEnabled(false);

    ObjectEditWidget::setObject(pipe);
    _pipe = pipe;

    QSignalBlocker b1(m_fromBox);
    QSignalBlocker b2(m_toBox);
    QSignalBlocker b3(m_pipeFileList);

    // Clear boxes
    m_fromBox->clear();
    m_toBox->clear();
    m_pipeFileList->clear();

    if (!pipe) return;

    // Load steps
    RamProject *project = pipe->project();
    if (!project) return;

    // find output and input steps
    for( int i = 0; i < project->steps()->count(); i++ )
    {
        RamStep *step = qobject_cast<RamStep*>( project->steps()->at(i) );
        const QString displayName = step->name();
        const QString uuid = step->uuid();
        m_fromBox->addItem(displayName, uuid );
        m_toBox->addItem(displayName, uuid );

        if (pipe->outputStep()->uuid() == uuid) m_fromBox->setCurrentIndex( m_fromBox->count() - 1);
        if (pipe->inputStep()->uuid() == uuid) m_toBox->setCurrentIndex( m_toBox->count() - 1);
    }

    // Load file types
    QList<QAction*> pipeFileActions = m_assignPipeFileMenu->actions();
    for (int i = 0; i < pipeFileActions.count(); i++)
        pipeFileActions.at(i)->setVisible(true);
    for (int i = 0; i < pipe->pipeFiles()->count(); i++)
        pipeFileAssigned( pipe->pipeFiles()->at(i));


    m_pipeFileList->setList( pipe->pipeFiles() );

    _objectConnections << connect(pipe->pipeFiles(), &RamObjectList::objectAdded, this, &PipeEditWidget::pipeFileAssigned);
    _objectConnections << connect(pipe->pipeFiles(), &RamObjectList::objectRemoved, this, &PipeEditWidget::pipeFileUnassigned);
    _objectConnections << connect(pipe->outputStep()->applications(), &RamObjectList::objectAdded, this, &PipeEditWidget::appChanged); //TODO Separate (un)assigned to connect app changed
    _objectConnections << connect(pipe->outputStep()->applications(), &RamObjectList::objectRemoved, this, &PipeEditWidget::appChanged);
    _objectConnections << connect(pipe->inputStep()->applications(), &RamObjectList::objectAdded, this, &PipeEditWidget::appChanged);
    _objectConnections << connect(pipe->inputStep()->applications(), &RamObjectList::objectRemoved, this, &PipeEditWidget::appChanged);

    this->setEnabled(Ramses::instance()->isProjectAdmin());
}

void PipeEditWidget::update()
{
    if (!_pipe) return;

    updating = true;

    RamProject *project = _pipe->project();
    if (!project) return;

    RamObject *inputStep = project->steps()->fromUuid( m_toBox->currentData().toString() );
    _pipe->setInputStep( qobject_cast<RamStep*>( inputStep ) );
    RamObject *outputStep = project->steps()->fromUuid( m_fromBox->currentData().toString() );
    _pipe->setOutputStep( qobject_cast<RamStep*>( outputStep ) );

    ObjectEditWidget::update();

    updating = false;
}

void PipeEditWidget::appChanged()
{
    // Load file types
}

void PipeEditWidget::assignPipeFile()
{
    if(!_pipe) return;
    QAction *pipeFileAction = (QAction*)sender();
    RamObject *pipeFileObj = RamPipeFile::pipeFile(  pipeFileAction->data().toString() );
    if (!pipeFileObj) return;
    _pipe->pipeFiles()->append(pipeFileObj);
}

void PipeEditWidget::pipeFileAssigned(RamObject *pipeFileObj)
{
    if (!pipeFileObj) return;
    if (pipeFileObj->uuid() == "") return;

    // Hide from assign list
    QList<QAction *> actions = m_assignPipeFileMenu->actions();
    for (int i = 1; i < actions.count(); i++)
    {
        if (actions[i]->data().toString() == pipeFileObj->uuid()) actions[i]->setVisible(false);
    }

}

void PipeEditWidget::pipeFileUnassigned(RamObject *pipeFileObj)
{
    //show in assign list
    QList<QAction *> actions = m_assignPipeFileMenu->actions();
    for (int i = 1; i < actions.count(); i++)
        if (actions[i]->data().toString() == pipeFileObj->uuid()) actions[i]->setVisible(true);
}

void PipeEditWidget::newPipeFile(RamObject *pipeFileObj)
{
    if (!pipeFileObj) return;
    if (pipeFileObj->uuid() == "") return;

    QAction *pipeFileAction = new QAction(pipeFileObj->shortName());
    pipeFileAction->setData(pipeFileObj->uuid());
    m_assignPipeFileMenu->addAction(pipeFileAction);
    QList<QMetaObject::Connection> c;
    c << connect(pipeFileObj, &RamObject::changed, this, &PipeEditWidget::pipeFileChanged);
    c << connect(pipeFileObj, &RamObject::removed, this, &PipeEditWidget::pipeFileRemoved);
    m_pipeFileConnections[pipeFileObj->uuid()] = c;
    connect(pipeFileAction, &QAction::triggered, this, &PipeEditWidget::assignPipeFile);
}

void PipeEditWidget::pipeFileChanged(RamObject *pipeFileObj)
{
    QList<QAction *> actions = m_assignPipeFileMenu->actions();
    for (int i = 1; i < actions.count(); i++)
        if (actions[i]->data().toString() == pipeFileObj->uuid()) actions[i]->setText(pipeFileObj->shortName());
}

void PipeEditWidget::pipeFileRemoved(RamObject *pipeFileObj)
{
    if (m_pipeFileConnections.contains(pipeFileObj->uuid()))
    {
        QList<QMetaObject::Connection> c = m_pipeFileConnections.value( pipeFileObj->uuid() );
        while (!c.isEmpty()) disconnect( c.takeLast() );
    }

    QList<QAction *> actions = m_assignPipeFileMenu->actions();
    for( int i = actions.count() -1 ; i >= 0 ; i--)
    {
        if (actions.at(i)->data().toString() == pipeFileObj->uuid()) actions.at(i)->deleteLater();
    }
}

void PipeEditWidget::createPipeFile()
{
    if(!_pipe) return;
    RamProject *project = Ramses::instance()->currentProject();
    if (!project) return;
    RamPipeFile *pipeFile = project->createPipeFile();
    _pipe->pipeFiles()->append(pipeFile);
}

void PipeEditWidget::setupUi()
{
    this->hideName();

    QLabel *fromLabel = new QLabel("From", this);
    mainFormLayout->addWidget(fromLabel, 2, 0);

    m_fromBox = new QComboBox(this);
    mainFormLayout->addWidget(m_fromBox, 2, 1);

    QLabel *toLabel = new QLabel("To", this);
    mainFormLayout->addWidget(toLabel, 3, 0);

    m_toBox = new QComboBox(this);
    mainFormLayout->addWidget(m_toBox, 3, 1);

    m_pipeFileList = new ObjectListEditWidget(true, this);
    m_pipeFileList->setEditMode(RamObjectListWidget::UnassignObjects);
    m_pipeFileList->setTitle("Files");
    mainLayout->addWidget(m_pipeFileList);

    // Add menu
    m_assignPipeFileMenu = new QMenu(this);
    QToolButton *addPipeFileButton = m_pipeFileList->addButton();
    addPipeFileButton->setPopupMode(QToolButton::InstantPopup);
    addPipeFileButton->setMenu(m_assignPipeFileMenu);
    m_newPipeFileAction = new QAction("New type...");
    m_assignPipeFileMenu->addAction(m_newPipeFileAction);
    m_assignPipeFileMenu->addSeparator();

}

void PipeEditWidget::connectEvents()
{
    connect(m_fromBox, SIGNAL(currentIndexChanged(int)), this, SLOT(update()));
    connect(m_toBox, SIGNAL(currentIndexChanged(int)), this, SLOT(update()));
    connect(Ramses::instance(), &Ramses::currentProjectChanged, this, &PipeEditWidget::populateMenus);
    connect(m_newPipeFileAction, &QAction::triggered, this, &PipeEditWidget::createPipeFile);
}

void PipeEditWidget::populateMenus(RamProject *project)
{
    // Disconnect previous connections
    while (!m_projectConnections.isEmpty()) disconnect( m_projectConnections.takeLast() );
    QMapIterator<QString, QList<QMetaObject::Connection>> i(m_pipeFileConnections);
    while (i.hasNext()) {
        i.next();
        QList<QMetaObject::Connection> c = i.value();
        while (!c.isEmpty()) disconnect( c.takeLast() );
    }
    m_pipeFileConnections.clear();

    // Clear
    QList<QAction *> actions = m_assignPipeFileMenu->actions();
        for (int i = actions.count() - 1; i >= 1 ; i--)
            actions[i]->deleteLater();

    // Get all pipefiles from project
    if (!project) return;

    for (int i = 0; i < project->pipeFiles()->count(); i++)
        newPipeFile( project->pipeFiles()->at(i) );

    connect( project->pipeFiles(), &RamObjectList::objectAdded, this, &PipeEditWidget::newPipeFile);
    connect( project->pipeFiles(), &RamObjectList::objectRemoved, this, &PipeEditWidget::pipeFileRemoved);
}

