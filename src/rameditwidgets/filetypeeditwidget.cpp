#include "filetypeeditwidget.h"

FileTypeEditWidget::FileTypeEditWidget(RamFileType *fileType) :
    ObjectEditWidget(fileType)
{
    setupUi();
    connectEvents();

    setObject(fileType);
}

RamFileType *FileTypeEditWidget::fileType() const
{
    return _fileType;
}

void FileTypeEditWidget::setObject(RamObject *obj)
{
    RamFileType *fileType = (RamFileType*)obj;

    this->setEnabled(false);

    ObjectEditWidget::setObject(fileType);
    _fileType = fileType;

    QSignalBlocker b1(ui_extensionsEdit);
    QSignalBlocker b2(ui_previewableBox);

    ui_extensionsEdit->setText("");
    ui_previewableBox->setChecked(false);

    if (!_fileType) return;

    ui_extensionsEdit->setText(_fileType->extensions().join(", "));
    ui_previewableBox->setChecked(_fileType->isPreviewable());

    this->setEnabled(Ramses::instance()->isProjectAdmin());
}

void FileTypeEditWidget::update()
{
    if (!_fileType) return;

    updating = true;

    _fileType->setExtensions(ui_extensionsEdit->text());
    _fileType->setPreviewable(ui_previewableBox->isChecked());

    ObjectEditWidget::update();

    updating = false;
}

void FileTypeEditWidget::updateExtensions()
{
    QStringList extensions = ui_extensionsEdit->text().split(",");
    QStringList fixedExtensions;
    for (int i = 0; i < extensions.count(); i++)
    {
        QString ext = extensions.at(i).trimmed();
        if (ext.startsWith(".")) ext = ext.remove(0,1);
        fixedExtensions << ext;
    }
    ui_extensionsEdit->setText( fixedExtensions.join(", "));
    update();
}

void FileTypeEditWidget::setupUi()
{
    ui_shortNameLabel->setText("Main extension");

    QLabel *extLabel = new QLabel("All extensions", this);
    ui_mainFormLayout->addWidget(extLabel, 3, 0);

    ui_extensionsEdit = new QLineEdit(this);
    ui_extensionsEdit->setPlaceholderText(".ext1, .ext2, .ext3...");
    ui_mainFormLayout->addWidget(ui_extensionsEdit, 3, 1);

    QLabel *previewableLabel = new QLabel("Previewable", this);
    ui_mainFormLayout->addWidget(previewableLabel, 4, 0);

    ui_previewableBox = new QCheckBox("This file can be used for previews\n(image, video...).", this);
    ui_mainFormLayout->addWidget(ui_previewableBox, 4, 1);
}

void FileTypeEditWidget::connectEvents()
{
    connect(ui_extensionsEdit, SIGNAL(editingFinished()), this, SLOT(updateExtensions()));
    connect(ui_extensionsEdit, &QLineEdit::editingFinished, this, &FileTypeEditWidget::update);
    connect(ui_previewableBox, &QCheckBox::clicked, this, &FileTypeEditWidget::update);

    monitorDbQuery("updateFileType");
}
