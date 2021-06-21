#include "filetypelistmanagerwidget.h"

FileTypeListManagerWidget::FileTypeListManagerWidget(QWidget *parent) :
    ObjectListManagerWidget(
        Ramses::instance()->fileTypes(),
        new FileTypeEditWidget(),
        "File types",
        parent )
{
}

void FileTypeListManagerWidget::createObject()
{
    Ramses::instance()->createFileType();
}

