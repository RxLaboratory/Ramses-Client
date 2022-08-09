#include "filetypelistmanagerwidget.h"

#include "ramses.h"

FileTypeListManagerWidget::FileTypeListManagerWidget(QWidget *parent) :
    ObjectListManagerWidget<RamFileType*, int>(
        Ramses::instance()->fileTypes(),
        "File types",
        QIcon(":icons/file"),
        parent )
{
    m_listEditWidget->setEditMode(ObjectListEditWidget<RamFileType*, int>::RemoveObjects);
}

RamFileType *FileTypeListManagerWidget::createObject()
{
    RamFileType *ft = new RamFileType(
                "NEW",
                "New file type");
    Ramses::instance()->fileTypes()->append(ft);
    ft->edit();
    return ft;
}

