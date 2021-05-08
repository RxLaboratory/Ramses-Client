#ifndef SHOTEDITWIDGET_H
#define SHOTEDITWIDGET_H

#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QComboBox>

#include "objecteditwidget.h"
#include "ramses.h"
#include "duqf-widgets/duqffolderdisplaywidget.h"

class ShotEditWidget : public ObjectEditWidget
{
    Q_OBJECT
public:
    ShotEditWidget(RamShot *shot = nullptr, QWidget *parent = nullptr);

public slots:
    void setObject(RamObject *obj) Q_DECL_OVERRIDE;

protected slots:
    void update() Q_DECL_OVERRIDE;

private slots:
    void moveShot();
    void framesChanged();
    void secondsChanged();
    void newSequence(RamObject *seq);
    void sequenceChanged(RamObject *o);
    void sequenceRemoved(RamObject *seq);

private:
    RamShot *_shot = nullptr;

    void setupUi();
    void connectEvents();

    DuQFFolderDisplayWidget *folderWidget;
    QDoubleSpinBox *secondsBox;
    QSpinBox *framesBox;
    QComboBox *sequencesBox;

    RamSequence *sequence();
    RamProject *project();
};

#endif // SHOTEDITWIDGET_H
