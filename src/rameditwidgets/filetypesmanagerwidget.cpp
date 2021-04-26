#include "filetypesmanagerwidget.h"

FileTypesManagerWidget::FileTypesManagerWidget(QWidget *parent): ListManagerWidget(parent)
{
        fileTypeWidget = new FileTypeEditWidget(this);
        this->setWidget(fileTypeWidget);
        fileTypeWidget->setEnabled(false);

        this->setRole(RamUser::ProjectAdmin);

        foreach(RamFileType *ft, Ramses::instance()->fileTypes()) newFileType(ft);

        connect(Ramses::instance(), &Ramses::newFileType, this, &FileTypesManagerWidget::newFileType);
}

void FileTypesManagerWidget::currentDataChanged(QVariant data)
{
    foreach(RamFileType *ft, Ramses::instance()->fileTypes())
    {
        if (ft->uuid() == data.toString())
        {
            fileTypeWidget->setFileType(ft);
            fileTypeWidget->setEnabled(true);
            return;
        }
    }
    fileTypeWidget->setEnabled(false);
}

void FileTypesManagerWidget::createItem()
{
    Ramses::instance()->createFileType();
}

void FileTypesManagerWidget::removeItem(QVariant data)
{
    Ramses::instance()->removeFileType(data.toString());
}

void FileTypesManagerWidget::newFileType(RamFileType *ft)
{
    if (ft->uuid() != "")
    {
        QListWidgetItem *fileTypeItem = new QListWidgetItem(ft->name());
        fileTypeItem->setData(Qt::UserRole, ft->uuid());
        this->addItem(fileTypeItem);
        connect(ft, &RamFileType::removed, this, &FileTypesManagerWidget::removeFileType);
        connect(ft, &RamFileType::changed, this, &FileTypesManagerWidget::fileTypeChanged);
    }
}

void FileTypesManagerWidget::removeFileType(RamObject *o)
{
    removeData(o->uuid());
}

void FileTypesManagerWidget::fileTypeChanged()
{
    RamFileType *ft = (RamFileType*)QObject::sender();
    updateItem(ft->uuid(), ft->name());
}
