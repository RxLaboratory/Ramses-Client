#include "pipefileeditwidget.h"

PipeFileEditWidget::PipeFileEditWidget(RamPipeFile *pipeFile, QWidget *parent):
    ObjectEditWidget(pipeFile, parent)
{
    setupUi();

    connectEvents();

    setObject(pipeFile);
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
    if (ft)
    {
        for (int i = 0; i < m_fileTypeBox->count(); i++)
        {
            if(m_fileTypeBox->itemData(i).toString() == ft->uuid())
            {
                m_fileTypeBox->setCurrentIndex(i);
                break;
            }
        }
    }

    this->setEnabled(true);
}

void PipeFileEditWidget::update()
{
    if (!m_pipeFile) return;

    if (!checkInput()) return;

    updating = true;

    RamFileType *ft = qobject_cast<RamFileType*>(m_fileTypeBox->currentObject());
    m_pipeFile->setFileType( ft );

    ObjectEditWidget::update();

    updating = false;
}

void PipeFileEditWidget::setupUi()
{
    nameLabel->hide();
    nameEdit->hide();

    QLabel *fileTypeLabel = new QLabel("File type", this);
    mainFormLayout->addWidget(fileTypeLabel, 2, 0);

    m_fileTypeBox = new RamObjectListComboBox(Ramses::instance()->fileTypes(), this);
    mainFormLayout->addWidget(m_fileTypeBox, 2, 1);

    QLabel *colorSpaceLabel = new QLabel("Color space", this);
    mainFormLayout->addWidget(colorSpaceLabel, 3, 0);

    m_colorSpaceBox = new RamObjectListComboBox(this);
    mainFormLayout->addWidget(m_colorSpaceBox, 3, 1);
}

void PipeFileEditWidget::connectEvents()
{
    connect(m_fileTypeBox, SIGNAL(currentObjectChanged(RamObject*)), this, SLOT(update()));
    connect(m_colorSpaceBox, SIGNAL(currentObjectChanged(RamObject*)), this, SLOT(update()));
}
