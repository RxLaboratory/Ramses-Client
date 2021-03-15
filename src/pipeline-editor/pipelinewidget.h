#ifndef PIPELINEWIDGET_H
#define PIPELINEWIDGET_H

#include <QCheckBox>

#include "ui_pipelinewidget.h"
#include "duqf-nodeview/duqfnodeview.h"
#include "duqf-nodeview/duqfnodescene.h"
#include "duqf-widgets/titlebar.h"
#include "duqf-widgets/duqfspinbox.h"

class PipelineWidget : public QWidget, private Ui::PipelineWidget
{
    Q_OBJECT

public:
    explicit PipelineWidget(QWidget *parent = nullptr);
signals:
    void reinitRequested();
    void closeRequested();
private:
    TitleBar *titleBar;
};

#endif // PIPELINEWIDGET_H
