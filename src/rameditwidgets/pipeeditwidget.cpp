#include "pipeeditwidget.h"


PipeEditWidget::PipeEditWidget(RamPipe *pipe, QWidget *parent) : ObjectEditWidget(pipe, parent)
{
    setupUi();
    connectEvents();

    setObject(pipe);
}

void PipeEditWidget::setObject(RamObject *pipeObj)
{
    RamPipe *pipe = qobject_cast<RamPipe*>( pipeObj );
    this->setEnabled(false);

    ObjectEditWidget::setObject(pipe);
    _pipe = pipe;

    QSignalBlocker b1(ui_fromBox);
    QSignalBlocker b2(ui_toBox);
    QSignalBlocker b3(ui_pipeFileList);

    // Clear boxes
    ui_fromBox->setList(nullptr);
    ui_toBox->setList(nullptr);
    ui_pipeFileList->setList(nullptr);

    if (!pipe) return;

    // Load steps
    RamProject *project = pipe->project();
    if (!project) return;

    // find output and input steps
    ui_fromBox->setList( project->steps() );
    ui_toBox->setList( project->steps() );
    ui_fromBox->setObject( pipe->outputStep() );
    ui_toBox->setObject( pipe->inputStep() );

    // Load file types
    ui_pipeFileList->setAssignList( project->pipeFiles() );
    ui_pipeFileList->setList( pipe->pipeFiles() );

    this->setEnabled(Ramses::instance()->isProjectAdmin());
}

void PipeEditWidget::update()
{
    if (!_pipe) return;

    updating = true;

    RamProject *project = _pipe->project();
    if (!project) return;

    RamStep *inputStep = reinterpret_cast<RamStep*>( ui_toBox->currentData().toULongLong() );
    _pipe->setInputStep( inputStep );
    RamStep *outputStep = reinterpret_cast<RamStep*>( ui_fromBox->currentData().toULongLong() );
    _pipe->setOutputStep( outputStep );

    ObjectEditWidget::update();

    updating = false;
}

void PipeEditWidget::appChanged()
{
    // Load file types
}

void PipeEditWidget::createPipeFile()
{
    if(!_pipe) return;
    RamProject *project = _pipe->project();
    if (!project) return;
    RamPipeFile *pipeFile = new RamPipeFile(
                "NEW",
                project);
    project->pipeFiles()->append(pipeFile);
    _pipe->pipeFiles()->append(pipeFile);
    pipeFile->edit();
}

void PipeEditWidget::setupUi()
{
    this->hideName();

    ui_commentEdit->hide();
    ui_commentLabel->hide();

    QLabel *fromLabel = new QLabel("From", this);
    ui_mainFormLayout->addWidget(fromLabel, 3, 0);

    ui_fromBox = new RamObjectListComboBox(this);
    ui_mainFormLayout->addWidget(ui_fromBox, 3, 1);

    QLabel *toLabel = new QLabel("To", this);
    ui_mainFormLayout->addWidget(toLabel, 4, 0);

    ui_toBox = new RamObjectListComboBox(this);
    ui_mainFormLayout->addWidget(ui_toBox, 4, 1);

    ui_pipeFileList = new ObjectListEditWidget(true, RamUser::ProjectAdmin, this);
    ui_pipeFileList->setEditMode(ObjectListEditWidget::UnassignObjects);
    ui_pipeFileList->setTitle("Files");
    ui_mainLayout->addWidget(ui_pipeFileList);
}

void PipeEditWidget::connectEvents()
{
    connect(ui_fromBox, SIGNAL(currentIndexChanged(int)), this, SLOT(update()));
    connect(ui_toBox, SIGNAL(currentIndexChanged(int)), this, SLOT(update()));
    connect(ui_pipeFileList, &ObjectListEditWidget::add, this, &PipeEditWidget::createPipeFile);
}

