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

    void setShot(RamShot *shot);

public slots:
    void setObject(RamObject *obj) Q_DECL_OVERRIDE;

protected slots:
    void update() Q_DECL_OVERRIDE;

private slots:
    void changeProject(RamProject *project);
    void moveShot();
    void framesChanged();
    void secondsChanged();

private:
    RamShot *_shot = nullptr;

    void setupUi();
    void connectEvents();

    DuQFFolderDisplayWidget *folderWidget;
    QDoubleSpinBox *secondsBox;
    QSpinBox *framesBox;
    RamObjectListComboBox *sequencesBox;
    StatusHistoryWidget *statusHistoryWidget;

    RamSequence *sequence();
    RamProject *project();
};

#endif // SHOTEDITWIDGET_H
