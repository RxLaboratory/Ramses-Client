#ifndef PIPEFILEEDITWIDGET_H
#define PIPEFILEEDITWIDGET_H

#include "objecteditwidget.h"
#include "ramobjectlistcombobox.h"
#include "rampipefile.h"

class PipeFileEditWidget : public ObjectEditWidget
{
    Q_OBJECT
public:
    PipeFileEditWidget(QWidget *parent = nullptr);
    PipeFileEditWidget(RamPipeFile *pipeFile, QWidget *parent = nullptr);

    RamPipeFile *pipeFile() const;

protected:
    virtual void reInit(RamObject *o) override;

private slots:
    void setFileType(RamObject *ft);
    void setCustomSettings();

private:
    RamPipeFile *m_pipeFile = nullptr;

    void setupUi();
    void connectEvents();

    RamObjectListComboBox *ui_fileTypeBox;
    //RamObjectListComboBox<RamObject*> *ui_colorSpaceBox;
    DuQFTextEdit *ui_customSettingsEdit;
};

#endif // PIPEFILEEDITWIDGET_H
