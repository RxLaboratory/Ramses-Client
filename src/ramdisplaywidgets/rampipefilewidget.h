#ifndef RAMPIPEFILEWIDGET_H
#define RAMPIPEFILEWIDGET_H

#include "ramobjectwidget.h"
#include "ramses.h"
#include "pipefileeditwidget.h"

class RamPipeFileWidget : public RamObjectWidget
{
    Q_OBJECT
public:
    RamPipeFileWidget(RamPipeFile *pipeFile, QWidget *parent = nullptr);
    RamPipeFile *pipeFile() const;

private:
    RamPipeFile *m_pipeFile;
};

#endif // RAMPIPEFILEWIDGET_H
