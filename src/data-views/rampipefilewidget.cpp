#include "rampipefilewidget.h"

RamPipeFileWidget::RamPipeFileWidget(RamPipeFile *pipeFile, QWidget *parent) :
    RamObjectWidget(pipeFile, parent)
{
    m_pipeFile = pipeFile;
    setUserEditRole(RamUser::ProjectAdmin);

    PipeFileEditWidget *ew = new PipeFileEditWidget(pipeFile, this);
    setEditWidget(ew);

    this->dockEditWidget()->setIcon(":/icons/connection");

    setIcon(":/icons/connection");

}

RamPipeFile *RamPipeFileWidget::pipeFile() const
{
    return m_pipeFile;
}
