#include "pipefileeditwidget.h"

PipeFileEditWidget::PipeFileEditWidget(RamPipeFile *pipeFile, QWidget *parent):
    ObjectEditWidget(pipeFile, parent)
{
    setupUi();
    setObject(pipeFile);
    connectEvents();
}

void PipeFileEditWidget::setObject(RamObject *obj)
{
    RamPipeFile *pipeFile = qobject_cast<RamPipeFile*>(obj);
    this->setEnabled(false);

    ObjectEditWidget::setObject(pipeFile);
    m_pipeFile = pipeFile;

    QSignalBlocker b1(ui_fileTypeBox);
    QSignalBlocker b2(ui_colorSpaceBox);

    //Reset values
    ui_fileTypeBox->setCurrentIndex(-1);
    ui_colorSpaceBox->setCurrentIndex(-1);

    if (!pipeFile) return;

    // Select file type
    RamFileType *ft = pipeFile->fileType();
    if(ft) ui_fileTypeBox->setObject( ft );

    this->setEnabled(Ramses::instance()->isProjectAdmin());
}

void PipeFileEditWidget::update()
{
    if (!m_pipeFile) return;

    if (!checkInput()) return;

    updating = true;

    RamFileType *ft = qobject_cast<RamFileType*>(ui_fileTypeBox->currentObject());
    if(ft) m_pipeFile->setFileType( ft );

    ObjectEditWidget::update();

    updating = false;
}

void PipeFileEditWidget::setupUi()
{
    ui_nameLabel->hide();
    ui_nameEdit->hide();

    QLabel *fileTypeLabel = new QLabel("File type", this);
    ui_mainFormLayout->addWidget(fileTypeLabel, 3, 0);

    ui_fileTypeBox = new RamObjectListComboBox(Ramses::instance()->fileTypes(), this);
    ui_mainFormLayout->addWidget(ui_fileTypeBox, 3, 1);

    QLabel *colorSpaceLabel = new QLabel("Color space", this);
    ui_mainFormLayout->addWidget(colorSpaceLabel, 4, 0);

    ui_colorSpaceBox = new RamObjectListComboBox(this);
    ui_mainFormLayout->addWidget(ui_colorSpaceBox, 4, 1);
}

void PipeFileEditWidget::connectEvents()
{
    connect(ui_fileTypeBox, SIGNAL(currentObjectChanged(RamObject*)), this, SLOT(update()));
    connect(ui_colorSpaceBox, SIGNAL(currentObjectChanged(RamObject*)), this, SLOT(update()));

    monitorDbQuery("updatePipeFile");
}
