#include "autoselectdoublespinbox.h"
#include "duqf-widgets/autoselectlineedit.h"

AutoSelectDoubleSpinBox::AutoSelectDoubleSpinBox(QWidget *parent): QDoubleSpinBox(parent)
{
    this->setLineEdit( new AutoSelectLineEdit(this ));
}
