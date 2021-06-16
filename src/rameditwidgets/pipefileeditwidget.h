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
    void setObject(RamObject *obj) Q_DECL_OVERRIDE;

protected slots:
    void update() Q_DECL_OVERRIDE;

private:
    RamPipeFile *m_pipeFile = nullptr;

    void setupUi();
    void connectEvents();

    RamObjectListComboBox *m_fileTypeBox;
    RamObjectListComboBox *m_colorSpaceBox;
};

#endif // PIPEFILEEDITWIDGET_H
