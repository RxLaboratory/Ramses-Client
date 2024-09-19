#include "dudoublespinbox.h"

DuDoubleSpinBox::DuDoubleSpinBox(QWidget *parent): QDoubleSpinBox(parent)
{
    _lineEdit = new DuLineEdit(this );
    this->setLineEdit( _lineEdit );
    setFrame(false);

    connect(this, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, [this]() { if (_editing) _edited = true; } );
    connect(this, &QDoubleSpinBox::editingFinished, this, &DuDoubleSpinBox::emitEdited);
}

void DuDoubleSpinBox::setValue(double d)
{
    _editing = false;
    QDoubleSpinBox::setValue(d);
    _editing = true;
}

void DuDoubleSpinBox::emitEdited()
{
    if (!_edited)
        return;
    _edited = false;
    emit edited(value());
}
