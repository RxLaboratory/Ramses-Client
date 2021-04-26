#ifndef PIPEEDITWIDGET_H
#define PIPEEDITWIDGET_H

#include <QComboBox>

#include "objecteditwidget.h"
#include "ramses.h"

/**
 * @brief The PipeEditWidget class is used to edit a RamPipe and is usually shown in the MainWindow Dock
 */
class PipeEditWidget : public ObjectEditWidget
{
public:
    PipeEditWidget(RamPipe *pipe = nullptr, QWidget *parent = nullptr);
    void setPipe(RamPipe *pipe);

private slots:
    void update() Q_DECL_OVERRIDE;
    void pipeChanged(RamObject *o);
    void appChanged();

private:
    RamPipe *_pipe = nullptr;

    void setupUi();
    void connectEvents();
    void updateFileTypes();

    QComboBox *fromBox;
    QComboBox *toBox;
    QComboBox *fileTypeBox;
};

#endif // PIPEEDITWIDGET_H
