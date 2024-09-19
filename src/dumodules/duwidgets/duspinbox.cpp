#include "duspinbox.h"

DuSpinBox::DuSpinBox(QWidget *parent): QSpinBox(parent)
{
    _lineEdit = new DuLineEdit(this );
    this->setLineEdit( _lineEdit );

    connect(this, QOverload<int>::of(&QSpinBox::valueChanged),
            this, [this]() { if (_editing) _edited = true; } );
    connect(this, &QSpinBox::editingFinished, this, &DuSpinBox::emitEdited);

}

void DuSpinBox::setValue(int v)
{
    _editing = false;
    QSpinBox::setValue(v);
    _editing = true;
}


void DuSpinBox::emitEdited()
{
    if (!_edited)
        return;
    _edited = false;
    emit edited(value());
}
