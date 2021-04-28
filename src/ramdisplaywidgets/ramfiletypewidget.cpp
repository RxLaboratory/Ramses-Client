#include "ramfiletypewidget.h"

#include "mainwindow.h"

RamFileTypeWidget::RamFileTypeWidget(RamFileType *fileType, QWidget *parent):
    RamObjectWidget(fileType, parent)
{
    m_fileType = fileType;

    FileTypeEditWidget *ftw = new FileTypeEditWidget(fileType, this);
    setEditWidget(ftw);
    MainWindow *mw = (MainWindow*)GuiUtils::appMainWindow();
    mw->addFileTypeEditDockWidget(this->dockEditWidget());

    setUserEditRole(RamUser::Admin);

    setIcon(":/icons/file");
}

RamFileType *RamFileTypeWidget::fileType() const
{
    return m_fileType;
}

void RamFileTypeWidget::setFileType(RamFileType *fileType)
{
    m_fileType = fileType;
}
