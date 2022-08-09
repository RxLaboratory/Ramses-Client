#ifndef PIPEFILELISTMANAGERWIDGET_H
#define PIPEFILELISTMANAGERWIDGET_H

#include "objectlistmanagerwidget.h"
#include "rampipefile.h"

class PipeFileListManagerWidget : public ObjectListManagerWidget<RamPipeFile *, RamProject*>
{
    Q_OBJECT
public:
    PipeFileListManagerWidget(QWidget *parent = nullptr);

protected slots:
    RamPipeFile *createObject() override;

private slots:
    void changeProject(RamProject *project);

private:
};

#endif // PIPEFILELISTMANAGERWIDGET_H
