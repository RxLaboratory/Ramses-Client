#include "duspinbox.h"

#include "duwidgets/dulineedit.h"

DuSpinBox::DuSpinBox(QWidget *parent): QSpinBox(parent)
{
    this->setLineEdit( new DuLineEdit(this ));
}
