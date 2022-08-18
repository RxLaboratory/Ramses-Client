#include "pipeeditwidget.h"

#include "rampipefile.h"

PipeEditWidget::PipeEditWidget(QWidget *parent) :
    ObjectEditWidget(parent)
{
    setupUi();
    connectEvents();
}

PipeEditWidget::PipeEditWidget(RamPipe *pipe, QWidget *parent) : ObjectEditWidget( parent)
{
    setupUi();
    connectEvents();
    setObject(pipe);
}

RamPipe *PipeEditWidget::pipe() const
{
    return m_pipe;
}

void PipeEditWidget::reInit(RamObject *o)
{
    m_pipe = qobject_cast<RamPipe*>(o);
    if (m_pipe)
    {
        // Load steps
        RamProject *project = m_pipe->project();
        if (!project) return;

        // find output and input steps
        QSignalBlocker b1(ui_toBox);
        QSignalBlocker b2(ui_fromBox);
        ui_fromBox->setList( project->steps() );
        ui_toBox->setList( project->steps() );

        ui_fromBox->setObject( m_pipe->outputStep() );
        ui_toBox->setObject( m_pipe->inputStep() );

        // Load file types
        ui_pipeFileList->setAssignList( project->pipeFiles() );
        ui_pipeFileList->setList( m_pipe->pipeFiles() );
    }
    else
    {
        // Clear boxes
        ui_fromBox->setList(nullptr);
        ui_toBox->setList(nullptr);
        ui_pipeFileList->setList(nullptr);
    }
}

void PipeEditWidget::appChanged()
{
    // Load file types
}

void PipeEditWidget::createPipeFile()
{
    if(!m_pipe) return;
    RamProject *project = m_pipe->project();
    if (!project) return;
    RamPipeFile *pipeFile = new RamPipeFile(
                "NEW",
                project);
    project->pipeFiles()->append(pipeFile);
    m_pipe->pipeFiles()->append(pipeFile);
    pipeFile->edit();
}

void PipeEditWidget::setInputStep(RamObject *step)
{
    Q_UNUSED(step)
    m_pipe->setInputStep( step );
}

void PipeEditWidget::setOutputStep(RamObject *step)
{
    if (!m_pipe) return;
    m_pipe->setOutputStep( step );
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
    connect(ui_fromBox, &RamObjectListComboBox::currentObjectChanged, this, &PipeEditWidget::setOutputStep);
    connect(ui_toBox,  &RamObjectListComboBox::currentObjectChanged, this, &PipeEditWidget::setInputStep);
    connect(ui_pipeFileList, &ObjectListEditWidget::add, this, &PipeEditWidget::createPipeFile);
}

