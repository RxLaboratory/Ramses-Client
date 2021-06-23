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

    QSignalBlocker b1(m_fileTypeBox);
    QSignalBlocker b2(m_colorSpaceBox);

    //Reset values
    m_fileTypeBox->setCurrentIndex(-1);
    m_colorSpaceBox->setCurrentIndex(-1);

    if (!pipeFile) return;

    // Select file type
    RamFileType *ft = pipeFile->fileType();
    if(ft) m_fileTypeBox->setObject( ft );

    this->setEnabled(true);
}

void PipeFileEditWidget::update()
{
    if (!m_pipeFile) return;

    if (!checkInput()) return;

    updating = true;

    RamFileType *ft = qobject_cast<RamFileType*>(m_fileTypeBox->currentObject());
    if(ft) m_pipeFile->setFileType( ft );

    ObjectEditWidget::update();

    updating = false;
}

void PipeFileEditWidget::setupUi()
{
    ui_nameLabel->hide();
    ui_nameEdit->hide();

    QLabel *fileTypeLabel = new QLabel("File type", this);
    ui_mainFormLayout->addWidget(fileTypeLabel, 2, 0);

    m_fileTypeBox = new RamObjectListComboBox(Ramses::instance()->fileTypes(), this);
    ui_mainFormLayout->addWidget(m_fileTypeBox, 2, 1);

    QLabel *colorSpaceLabel = new QLabel("Color space", this);
    ui_mainFormLayout->addWidget(colorSpaceLabel, 3, 0);

    m_colorSpaceBox = new RamObjectListComboBox(this);
    ui_mainFormLayout->addWidget(m_colorSpaceBox, 3, 1);
}

void PipeFileEditWidget::connectEvents()
{
    connect(m_fileTypeBox, SIGNAL(currentObjectChanged(RamObject*)), this, SLOT(update()));
    connect(m_colorSpaceBox, SIGNAL(currentObjectChanged(RamObject*)), this, SLOT(update()));
}
