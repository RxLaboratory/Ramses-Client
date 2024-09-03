#include "dulineedit.h"

DuLineEdit::DuLineEdit(QWidget *parent): QLineEdit(parent)
{
    _selectOnMousePress = false;
}

void DuLineEdit::focusInEvent(QFocusEvent *event)
{
    QLineEdit::focusInEvent(event);
    if (!_autoSelect) return;
    this->selectAll();
    _selectOnMousePress = true;
}

void DuLineEdit::mousePressEvent(QMouseEvent *me)
{
    QLineEdit::mousePressEvent(me);
    if (!_autoSelect) return;
    if(_selectOnMousePress) {
        this->selectAll();
        _selectOnMousePress = false;
    }
}

bool DuLineEdit::autoSelect() const
{
    return _autoSelect;
}

void DuLineEdit::setAutoSelect(bool newAutoSelect)
{
    _autoSelect = newAutoSelect;
}
