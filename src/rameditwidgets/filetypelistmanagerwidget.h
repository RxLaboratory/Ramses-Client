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
    void createObject() override;
};

#endif // FILETYPELISTMANAGERWIDGET_H
