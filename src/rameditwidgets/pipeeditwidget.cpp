#include "pipeeditwidget.h"


PipeEditWidget::PipeEditWidget(RamPipe *pipe, QWidget *parent) : ObjectEditWidget(pipe, parent)
{
    setupUi();
    connectEvents();

    setPipe(pipe);
}

void PipeEditWidget::setPipe(RamPipe *pipe)
{
    this->setEnabled(false);

    setObject(pipe);
    _pipe = pipe;

    QSignalBlocker b1(fromBox);
    QSignalBlocker b2(toBox);
    QSignalBlocker b3(fileTypeBox);

    // Clear boxes
    fromBox->clear();
    toBox->clear();
    fileTypeBox->clear();

    if (!pipe) return;

    // Load steps
    RamProject *project = Ramses::instance()->project( pipe->outputStep()->projectUuid() );
    if (!project) return;

    // find output and input steps
    for( int i = 0; i < project->steps()->count(); i++ )
    {
        RamStep *step = qobject_cast<RamStep*>( project->steps()->at(i) );
        const QString displayName = step->name();
        const QString uuid = step->uuid();
        fromBox->addItem(displayName, uuid );
        toBox->addItem(displayName, uuid );

        if (pipe->outputStep()->uuid() == uuid) fromBox->setCurrentIndex( fromBox->count() - 1);
        if (pipe->inputStep()->uuid() == uuid) toBox->setCurrentIndex( toBox->count() - 1);
    }

    // Load file types
    updateFileTypes();

    _objectConnections << connect(pipe, &RamPipe::changed, this, &PipeEditWidget::pipeChanged);
    _objectConnections << connect(pipe->outputStep()->applications(), &RamObjectList::objectAdded, this, &PipeEditWidget::appChanged);
    _objectConnections << connect(pipe->outputStep()->applications(), &RamObjectList::objectRemoved, this, &PipeEditWidget::appChanged);
    _objectConnections << connect(pipe->inputStep()->applications(), &RamObjectList::objectAdded, this, &PipeEditWidget::appChanged);
    _objectConnections << connect(pipe->inputStep()->applications(), &RamObjectList::objectRemoved, this, &PipeEditWidget::appChanged);

    this->setEnabled(Ramses::instance()->isProjectAdmin());
}

void PipeEditWidget::update()
{
    if (!_pipe) return;

    RamProject *project = Ramses::instance()->project( _pipe->outputStep()->projectUuid() );
    if (!project) return;

    _pipe->setFileType( Ramses::instance()->fileType( fileTypeBox->currentData().toString() ) );
    RamObject *inputStep = project->steps()->fromUuid( toBox->currentData().toString() );
    _pipe->setInputStep( qobject_cast<RamStep*>( inputStep ) );
    RamObject *outputStep = project->steps()->fromUuid( fromBox->currentData().toString() );
    _pipe->setOutputStep( qobject_cast<RamStep*>( outputStep ) );

    _pipe->update();
}

void PipeEditWidget::pipeChanged(RamObject *o)
{
    Q_UNUSED(o);
    setPipe(_pipe);
}

void PipeEditWidget::appChanged()
{
    // Load file types
    updateFileTypes();
}

void PipeEditWidget::setupUi()
{
    this->hideName();

    QLabel *fromLabel = new QLabel("From", this);
    mainFormLayout->addWidget(fromLabel, 2, 0);

    fromBox = new QComboBox(this);
    mainFormLayout->addWidget(fromBox, 2, 1);

    QLabel *toLabel = new QLabel("To", this);
    mainFormLayout->addWidget(toLabel, 3, 0);

    toBox = new QComboBox(this);
    mainFormLayout->addWidget(toBox, 3, 1);

    QLabel *fileLabel = new QLabel("File type", this);
    mainFormLayout->addWidget(fileLabel, 4, 0);

    fileTypeBox = new QComboBox(this);
    mainFormLayout->addWidget(fileTypeBox, 4, 1);
}

void PipeEditWidget::connectEvents()
{
    connect(fromBox, SIGNAL(currentIndexChanged(int)), this, SLOT(update()));
    connect(toBox, SIGNAL(currentIndexChanged(int)), this, SLOT(update()));
    connect(fileTypeBox, SIGNAL(currentIndexChanged(int)), this, SLOT(update()));
}

void PipeEditWidget::updateFileTypes()
{
    fileTypeBox->clear();
    // File types
    fileTypeBox->addItem("None / Any", "");

    RamFileType *currentFT = _pipe->fileType();

    QStringList uuids;

    for ( RamObject *outputFileType: _pipe->outputStep()->outputFileTypes() )
    {

        QString uuid = outputFileType->uuid();
        //already added
        if (uuids.contains( uuid )) continue;

        for ( RamObject *inputFileType: _pipe->inputStep()->inputFileTypes() )
        {
            if (outputFileType->uuid() == inputFileType->uuid())
            {
                uuids << uuid;
                fileTypeBox->addItem( outputFileType->name() + " (" + outputFileType->shortName() + ")", uuid );
                if (currentFT) if (currentFT->uuid() == uuid)
                    fileTypeBox->setCurrentIndex( fileTypeBox->count() -1 );
                break;
            }
        }
    }
}
