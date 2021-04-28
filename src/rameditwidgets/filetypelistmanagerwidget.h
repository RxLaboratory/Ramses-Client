#ifndef FILETYPELISTMANAGERWIDGET_H
#define FILETYPELISTMANAGERWIDGET_H

#include "objectlistmanagerwidget.h"
#include "filetypeeditwidget.h"

class FileTypeListManagerWidget : public ObjectListManagerWidget
{
    Q_OBJECT
public:
    FileTypeListManagerWidget(QWidget *parent = nullptr);

protected slots:
    void createObject() Q_DECL_OVERRIDE;
};

#endif // FILETYPELISTMANAGERWIDGET_H
