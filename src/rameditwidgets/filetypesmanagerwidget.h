#ifndef FILETYPESMANAGERWIDGET_H
#define FILETYPESMANAGERWIDGET_H

#include "listmanagerwidget.h"
#include "filetypeeditwidget.h"

class FileTypesManagerWidget : public ListManagerWidget
{
    Q_OBJECT
public:
    FileTypesManagerWidget(QWidget *parent = nullptr);

protected slots:
    void currentDataChanged(QVariant data) Q_DECL_OVERRIDE;
    void createItem() Q_DECL_OVERRIDE;
    void removeItem(QVariant data) Q_DECL_OVERRIDE;

private slots:
    void newFileType(RamFileType *ft);
    void removeFileType(RamObject *o);
    void fileTypeChanged();

private:
    FileTypeEditWidget *fileTypeWidget;
};

#endif // FILETYPESMANAGERWIDGET_H
