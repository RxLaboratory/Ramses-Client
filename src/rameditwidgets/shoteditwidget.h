#ifndef SHOTEDITWIDGET_H
#define SHOTEDITWIDGET_H

#include <QDoubleSpinBox>
#include <QSpinBox>

#include "objecteditwidget.h"
#include "ramses.h"
#include "statushistorywidget.h"
#include "ramobjectlistcombobox.h"
#include "duqf-widgets/duqffolderdisplaywidget.h"

class ShotEditWidget : public ObjectEditWidget
{
    Q_OBJECT
public:
    ShotEditWidget(QWidget *parent = nullptr);
    ShotEditWidget(RamShot *shot, QWidget *parent = nullptr);

public slots:
    void setObject(RamObject *obj) Q_DECL_OVERRIDE;

protected slots:
    void update() Q_DECL_OVERRIDE;

private slots:
    void framesChanged();
    void secondsChanged();

private:
    RamShot *_shot = nullptr;

    void setupUi();
    void connectEvents();

    DuQFFolderDisplayWidget *ui_folderWidget;
    QDoubleSpinBox *ui_secondsBox;
    QSpinBox *ui_framesBox;
    RamObjectListComboBox *ui_sequencesBox;
};

#endif // SHOTEDITWIDGET_H
