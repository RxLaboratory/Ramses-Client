#ifndef PIPEEDITWIDGET_H
#define PIPEEDITWIDGET_H

#include <QComboBox>
#include <QMenu>

#include "objecteditwidget.h"
#include "objectlisteditwidget.h"
#include "ramobjectlistcombobox.h"
#include "ramses.h"

/**
 * @brief The PipeEditWidget class is used to edit a RamPipe and is usually shown in the MainWindow Dock
 */
class PipeEditWidget : public ObjectEditWidget
{
public:
    PipeEditWidget(RamPipe *pipe = nullptr, QWidget *parent = nullptr);

public slots:
    void setObject(RamObject *pipeObj) override;

protected slots:
    void update() override;

private slots:
    void appChanged();
    void createPipeFile();

private:
    RamPipe *_pipe = nullptr;

    void setupUi();
    void connectEvents();
    void populateMenus(RamProject *project);

    RamObjectListComboBox *ui_fromBox;
    RamObjectListComboBox *ui_toBox;
    ObjectListEditWidget *ui_pipeFileList;
};

#endif // PIPEEDITWIDGET_H
