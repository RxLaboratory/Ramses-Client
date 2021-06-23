#ifndef SHOTEDITWIDGET_H
#define SHOTEDITWIDGET_H

#include <QDoubleSpinBox>
#include <QSpinBox>

#include "objecteditwidget.h"
#include "ramses.h"
#include "statushistorywidget.h"
#include "ramobjectlistcombobox.h"
#include "duqf-widgets/duqffolderdisplaywidget.h"
#include "duqf-widgets/autoselectdoublespinbox.h"
#include "duqf-widgets/autoselectspinbox.h"

class ShotEditWidget : public ObjectEditWidget
{
    Q_OBJECT
public:
    ShotEditWidget(QWidget *parent = nullptr);
    ShotEditWidget(RamShot *shot, QWidget *parent = nullptr);

public slots:
    void setObject(RamObject *obj) override;

protected slots:
    void update() override;

private slots:
    void framesChanged();
    void secondsChanged();

private:
    RamShot *_shot = nullptr;

    void setupUi();
    void connectEvents();

    DuQFFolderDisplayWidget *ui_folderWidget;
    AutoSelectDoubleSpinBox *ui_secondsBox;
    AutoSelectSpinBox *ui_framesBox;
    RamObjectListComboBox *ui_sequencesBox;
};

#endif // SHOTEDITWIDGET_H
