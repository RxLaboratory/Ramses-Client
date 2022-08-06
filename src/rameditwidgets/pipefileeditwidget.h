#ifndef PIPEFILEEDITWIDGET_H
#define PIPEFILEEDITWIDGET_H

#include "objecteditwidget.h"
#include "ramobjectlistcombobox.h"
#include "rampipefile.h"

class PipeFileEditWidget : public ObjectEditWidget
{
    Q_OBJECT
public:
    PipeFileEditWidget(RamPipeFile *pipeFile = nullptr, QWidget *parent = nullptr);

public slots:
    void setObject(RamObject *obj) override;

protected slots:
    void update() override;

private slots:
    void setFileType();

private:
    RamPipeFile *m_pipeFile = nullptr;

    void setupUi();
    void connectEvents();

    RamObjectListComboBox<RamFileType *> *ui_fileTypeBox;
    RamObjectListComboBox<RamObject*> *ui_colorSpaceBox;
    DuQFTextEdit *ui_customSettingsEdit;
};

#endif // PIPEFILEEDITWIDGET_H
