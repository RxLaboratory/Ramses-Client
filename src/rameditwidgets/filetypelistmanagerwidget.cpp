#include "filetypelistmanagerwidget.h"

FileTypeListManagerWidget::FileTypeListManagerWidget(QWidget *parent) :
    ObjectListManagerWidget(
        Ramses::instance()->fileTypes(),
        new FileTypeEditWidget(),
        "File types",
        QIcon(":icons/file"),
        parent )
{
    m_listEditWidget->setEditMode(ObjectListEditWidget::RemoveObjects);
}

RamObject *FileTypeListManagerWidget::createObject()
{
    RamFileType *ft = new RamFileType(
                "NEW",
                "New file type");
    Ramses::instance()->fileTypes()->append(ft);
    editObject(ft);
    return ft;
}

