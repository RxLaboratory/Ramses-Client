#include "dudateedit.h"

DuDateEdit::DuDateEdit(QWidget *parent):
    QDateEdit(parent)
{
    _lineEdit = new DuLineEdit(this );
    this->setLineEdit( _lineEdit );
    setFrame(false);

    connect(this, &QDateEdit::dateChanged,
            this, [this]() { if (_editing) _edited = true; } );
    connect(this, &QDateEdit::editingFinished, this, &DuDateEdit::emitEdited);
}

void DuDateEdit::setDate(const QDate &date)
{
    _editing = false;
    QDateEdit::setDate(date);
    _editing = true;
}

void DuDateEdit::emitEdited()
{
    if (!_edited)
        return;
    _edited = false;
    emit edited(date());
}

