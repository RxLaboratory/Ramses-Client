#include "autoselectdoublespinbox.h"
#include "duwidgets/autoselectlineedit.h"

AutoSelectDoubleSpinBox::AutoSelectDoubleSpinBox(QWidget *parent): QDoubleSpinBox(parent)
{
    this->setLineEdit( new AutoSelectLineEdit(this ));
}
