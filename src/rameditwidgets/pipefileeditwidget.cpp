#include "pipefileeditwidget.h"

#include "ramses.h"

PipeFileEditWidget::PipeFileEditWidget(QWidget *parent) :
    ObjectEditWidget(parent)
{
    setupUi();
    connectEvents();
}


PipeFileEditWidget::PipeFileEditWidget(RamPipeFile *pipeFile, QWidget *parent):
    ObjectEditWidget(parent)
{
    setupUi();
    connectEvents();
    setObject(pipeFile);
}

RamPipeFile *PipeFileEditWidget::pipeFile() const
{
    return m_pipeFile;
}

void PipeFileEditWidget::reInit(RamObject *o)
{
    m_pipeFile = qobject_cast<RamPipeFile*>(o);
    if (m_pipeFile)
    {
        // Select file type
        RamFileType *ft = m_pipeFile->fileType();
        if(ft) ui_fileTypeBox->setObject( ft );

        ui_customSettingsEdit->setPlainText( m_pipeFile->customSettings() );
    }
    else
    {
        //Reset values
        ui_fileTypeBox->setCurrentIndex(-1);
        //ui_colorSpaceBox->setCurrentIndex(-1);
        ui_customSettingsEdit->setText("");
    }
}

void PipeFileEditWidget::setFileType(RamFileType *ft)
{
    if (!m_pipeFile) return;
    if(ft) m_pipeFile->setFileType( ft );
    //ui_customSettingsEdit->setPlainText( m_pipeFile->customSettings() );
}

void PipeFileEditWidget::setCustomSettings()
{
    m_pipeFile->setCustomSettings(ui_customSettingsEdit->toPlainText());
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

    //ui_colorSpaceBox = new RamObjectListComboBox<RamObject*>(this);
    //ui_mainFormLayout->addWidget(ui_colorSpaceBox, 4, 1);

    QLabel *customSettingsLabel = new QLabel("Custom settings", this);
    ui_mainFormLayout->addWidget(customSettingsLabel, 5, 0);

    ui_customSettingsEdit = new DuQFTextEdit();
    ui_customSettingsEdit->setUseMarkdown(false);
    ui_mainFormLayout->addWidget(ui_customSettingsEdit, 5, 1);

    ui_mainLayout->addStretch(100);
}

void PipeFileEditWidget::connectEvents()
{
    connect(ui_fileTypeBox, SIGNAL(currentObjectChanged(RamObject*)), this, SLOT(setFileType(RamObject*)));
    //connect(ui_colorSpaceBox, SIGNAL(currentObjectChanged(RamObject*)), this, SLOT(update()));
    connect(ui_customSettingsEdit, SIGNAL(editingFinished()), this, SLOT(setCustomSettings()));
}
