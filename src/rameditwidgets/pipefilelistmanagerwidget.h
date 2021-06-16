#ifndef PIPEFILELISTMANAGERWIDGET_H
#define PIPEFILELISTMANAGERWIDGET_H

#include "objectlistmanagerwidget.h"
#include "pipefileeditwidget.h"

class PipeFileListManagerWidget : public ObjectListManagerWidget
{
    Q_OBJECT
public:
    PipeFileListManagerWidget(QWidget *parent = nullptr);

protected slots:
    void createObject() Q_DECL_OVERRIDE;

private slots:
    void changeProject(RamProject *project);

private:
};

#endif // PIPEFILELISTMANAGERWIDGET_H
