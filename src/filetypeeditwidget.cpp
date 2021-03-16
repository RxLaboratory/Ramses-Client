#include "filetypeeditwidget.h"

FileTypeEditWidget::FileTypeEditWidget(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);

    connect(updateButton, SIGNAL(clicked()), this, SLOT(update()));
    connect(revertButton, SIGNAL(clicked()), this, SLOT(revert()));
    connect(shortNameEdit, &QLineEdit::textChanged, this, &FileTypeEditWidget::checkInput);
    connect(DBInterface::instance(),&DBInterface::newLog, this, &FileTypeEditWidget::dbiLog);

    this->setEnabled(false);
}

RamFileType *FileTypeEditWidget::fileType() const
{
    return _fileType;
}

void FileTypeEditWidget::setFileType(RamFileType *fileType)
{
    disconnect(_currentFileTypeConnection);

    _fileType = fileType;

    nameEdit->setText("");
    shortNameEdit->setText("");
    extensionsEdit->setText("");
    previewableBox->setChecked(false);

    if (!fileType) return;

    nameEdit->setText(fileType->name());
    shortNameEdit->setText(fileType->shortName());
    extensionsEdit->setText(fileType->extensions().join(", "));
    previewableBox->setChecked(fileType->isPreviewable());

    this->setEnabled(Ramses::instance()->isProjectAdmin());

    _currentFileTypeConnection = connect(fileType, &RamObject::removed, this, &FileTypeEditWidget::fileTypeRemoved);
}

void FileTypeEditWidget::update()
{
    if (!_fileType) return;

    this->setEnabled(false);

    if (!checkInput())
    {
        this->setEnabled(true);
        return;
    }

    _fileType->setName(nameEdit->text());
    _fileType->setShortName(shortNameEdit->text());
    _fileType->setExtensions(extensionsEdit->text());
    _fileType->setPreviewable(previewableBox->isChecked());

    _fileType->update();

    this->setEnabled(true);
}

void FileTypeEditWidget::revert()
{
    setFileType(_fileType);
}

bool FileTypeEditWidget::checkInput()
{
    if (!_fileType) return false;

    if (shortNameEdit->text() == "")
    {
        statusLabel->setText("Short name cannot be empty!");
        updateButton->setEnabled(false);
        return false;
    }

    statusLabel->setText("");
    updateButton->setEnabled(true);
    return true;
}

void FileTypeEditWidget::fileTypeRemoved(RamObject *o)
{
    Q_UNUSED(o);
    setFileType(nullptr);
}

void FileTypeEditWidget::dbiLog(DuQFLog m)
{
    if (m.type() != DuQFLog::Debug) statusLabel->setText(m.message());
}
