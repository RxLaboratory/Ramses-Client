#include "testcustomwidgets.h"

TestCustomWidgets::TestCustomWidgets(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);

    DuQFDoubleSlider *ds = new DuQFDoubleSlider();
    ds->setFormat("Value: %v");
    formLayout->addRow(new QLabel("Double Slider"), ds);
    DuQFSlider *ps = new DuQFSlider();
    ps->setFormat("Value: %v");
    formLayout->addRow(new QLabel("Slider"), ps);
    DuQFSpinBox *sp = new DuQFSpinBox();
    sp->setPrefix("Width: ");
    sp->setSuffix("px");
    formLayout->addRow(new QLabel("Spin Box"), sp);
    DuQFDoubleSpinBox *dp = new DuQFDoubleSpinBox();
    dp->setPrefix("Height: ");
    dp->setSuffix("cm");
    formLayout->addRow(new QLabel("Double Spin Box"), dp);
}
