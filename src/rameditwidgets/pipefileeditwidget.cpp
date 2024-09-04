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
        ui_fileTypeBox->setObject( ft );

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

void PipeFileEditWidget::setFileType(RamObject *ft)
{
    if (!m_pipeFile || m_reinit) return;
    if (ft) m_pipeFile->setFileType( RamFileType::c( ft ) );
    //ui_customSettingsEdit->setPlainText( m_pipeFile->customSettings() );
}

void PipeFileEditWidget::setCustomSettings()
{
    if (!m_pipeFile || m_reinit) return;
    m_pipeFile->setCustomSettings(ui_customSettingsEdit->toPlainText());
}

void PipeFileEditWidget::setupUi()
{
    ui_nameLabel->hide();
    ui_nameEdit->hide();

    QLabel *fileTypeLabel = new QLabel(tr("File type"), this);
    ui_mainFormLayout->addWidget(fileTypeLabel, 3, 0);

    ui_fileTypeBox = new RamObjectComboBox(this);
    ui_fileTypeBox->setSortMode(RamObject::ShortName);
    ui_fileTypeBox->setObjectModel(Ramses::instance()->fileTypes());
    ui_mainFormLayout->addWidget(ui_fileTypeBox, 3, 1);

    QLabel *colorSpaceLabel = new QLabel(tr("Color space"), this);
    ui_mainFormLayout->addWidget(colorSpaceLabel, 4, 0);

    //ui_colorSpaceBox = new RamObjectListComboBox<RamObject*>(this);
    //ui_mainFormLayout->addWidget(ui_colorSpaceBox, 4, 1);

    ui_mainLayout->addStretch(100);

    auto settingsWidget = new QWidget(this);
    auto settingsLayout = new QVBoxLayout(settingsWidget);
    settingsLayout->setContentsMargins(3,6,6,6);
    settingsLayout->setSpacing(3);

    settingsLayout->addWidget(new QLabel(
        "<b>"+tr("Import settings")+"</b>"
        ));

    ui_customSettingsEdit = new DuRichTextEdit();
    ui_customSettingsEdit->setProperty("class", "duBlock");
    ui_customSettingsEdit->setUseMarkdown(false);
    ui_customSettingsEdit->setUseMarkdown(false);
    settingsLayout->addWidget(ui_customSettingsEdit);

    ui_tabWidget->addTab(settingsWidget, DuIcon(":/icons/settings"), "");
    ui_tabWidget->setTabToolTip(2, tr("Import settings"));
}

void PipeFileEditWidget::connectEvents()
{
    connect(ui_fileTypeBox, &RamObjectComboBox::currentObjectChanged, this, &PipeFileEditWidget::setFileType);
    //connect(ui_colorSpaceBox, SIGNAL(currentObjectChanged(RamObject*)), this, SLOT(update()));
    connect(ui_customSettingsEdit, SIGNAL(editingFinished()), this, SLOT(setCustomSettings()));
}
