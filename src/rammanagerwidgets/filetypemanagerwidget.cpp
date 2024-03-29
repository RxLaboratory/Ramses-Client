#include "filetypemanagerwidget.h"

#include "ramses.h"
#include "duqf-widgets/duicon.h"

FileTypeManagerWidget::FileTypeManagerWidget(QWidget *parent) :
    ObjectManagerWidget(
        Ramses::instance()->fileTypes(),
        "File types",
        DuIcon(":icons/file"),
        parent )
{
    ui_listWidget->setEditMode(ObjectListWidget::RemoveObjects);
    ui_listWidget->setSortMode(RamAbstractObject::ShortName);
    ui_listWidget->sort();
}

RamFileType *FileTypeManagerWidget::createObject()
{
    RamFileType *ft = new RamFileType(
                "NEW",
                "New file type");
    //Ramses::instance()->fileTypes()->append(ft);
    ft->edit();
    return ft;
}

