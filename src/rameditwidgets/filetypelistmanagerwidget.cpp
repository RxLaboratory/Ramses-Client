#include "filetypelistmanagerwidget.h"

#include "ramses.h"

FileTypeListManagerWidget::FileTypeListManagerWidget(QWidget *parent) :
    ObjectListManagerWidget(
        Ramses::instance()->fileTypes(),
        "File types",
        QIcon(":icons/file"),
        parent )
{
    m_listEditWidget->setEditMode(ObjectListEditWidget::RemoveObjects);
    m_listEditWidget->setSortMode(RamObjectList::ShortName);
    m_listEditWidget->sort();
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

