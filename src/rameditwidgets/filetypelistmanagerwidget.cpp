#include "filetypelistmanagerwidget.h"

FileTypeListManagerWidget::FileTypeListManagerWidget(QWidget *parent) :
    ObjectListManagerWidget(
        Ramses::instance()->fileTypes(),
        new FileTypeEditWidget(),
        "File types",
        parent )
{
    this->setContainingType(RamObject::FileType);
}

void FileTypeListManagerWidget::createObject()
{
    Ramses::instance()->createFileType();
}

