#ifndef FILETYPELISTMANAGERWIDGET_H
#define FILETYPELISTMANAGERWIDGET_H

#include "objectlistmanagerwidget.h"
#include "ramfiletype.h"

class FileTypeListManagerWidget : public ObjectListManagerWidget
{
    Q_OBJECT
public:
    FileTypeListManagerWidget(QWidget *parent = nullptr);

protected slots:
    RamFileType *createObject() override;
};

#endif // FILETYPELISTMANAGERWIDGET_H
