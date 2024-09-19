#ifndef DUDATEEDIT_H
#define DUDATEEDIT_H

#include <QDateEdit>
#include "duwidgets/dulineedit.h"

class DuDateEdit : public QDateEdit
{
    Q_OBJECT
public:
    DuDateEdit(QWidget *parent = nullptr);

    void setDate(const QDate &date);

    bool autoSelect() const { return _lineEdit->autoSelect(); }
    void setAutoSelect(bool newAutoSelect) { _lineEdit->setAutoSelect(newAutoSelect); }

signals:
    /**
     * @brief edited Emitted on editingFinished
     * IF AND ONLY IF the value has actually changed.
     * Emits the new value.
     */
    void edited(QDate);

private:
    void emitEdited();
    bool _edited = false;
    bool _editing = true;
    DuLineEdit *_lineEdit;
};

#endif // DUDATEEDIT_H
