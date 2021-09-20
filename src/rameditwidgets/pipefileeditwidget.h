#ifndef PIPEFILEEDITWIDGET_H
#define PIPEFILEEDITWIDGET_H

#include "objecteditwidget.h"
#include "ramses.h"
#include "ramobjectlistcombobox.h"

class PipeFileEditWidget : public ObjectEditWidget
{
    Q_OBJECT
public:
    PipeFileEditWidget(RamPipeFile *pipeFile = nullptr, QWidget *parent = nullptr);

public slots:
    void setObject(RamObject *obj) override;

protected slots:
    void update() override;

private:
    RamPipeFile *m_pipeFile = nullptr;

    void setupUi();
    void connectEvents();

    RamObjectListComboBox *ui_fileTypeBox;
    RamObjectListComboBox *ui_colorSpaceBox;
    QLineEdit *ui_customSettingsEdit;
};

#endif // PIPEFILEEDITWIDGET_H
