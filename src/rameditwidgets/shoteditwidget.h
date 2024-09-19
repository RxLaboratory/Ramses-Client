#ifndef SHOTEDITWIDGET_H
#define SHOTEDITWIDGET_H

#include <QDoubleSpinBox>
#include <QSpinBox>

#include "objecteditwidget.h"
#include "duwidgets/duqffolderdisplaywidget.h"
#include "duwidgets/dudoublespinbox.h"
#include "duwidgets/duspinbox.h"
#include "objectlistwidget.h"
#include "ramshot.h"

class ShotEditWidget : public ObjectEditWidget
{
    Q_OBJECT
public:
    ShotEditWidget(QWidget *parent = nullptr);
    ShotEditWidget(RamShot *shot, QWidget *parent = nullptr);

    RamShot *shot();

protected:
    virtual void reInit(RamObject *o) override;

private slots:
    void setDuration();
    void setSequence(RamObject *seq);
    void framesChanged();
    void secondsChanged();

private:
    RamShot *m_shot = nullptr;
    // Keep not rounded duration!
    qreal m_duration;

    void setupUi();
    void connectEvents();

    DuQFFolderDisplayWidget *ui_folderWidget;
    DuDoubleSpinBox *ui_secondsBox;
    DuSpinBox *ui_framesBox;
    RamObjectComboBox *ui_sequencesBox;
    ObjectListWidget *ui_assetList;
};

#endif // SHOTEDITWIDGET_H
