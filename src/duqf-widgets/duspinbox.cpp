#include "duspinbox.h"

#include "duqf-widgets/dulineedit.h"

DuSpinBox::DuSpinBox(QWidget *parent): QSpinBox(parent)
{
    this->setLineEdit( new DuLineEdit(this ));
}
