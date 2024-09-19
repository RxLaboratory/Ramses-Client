#ifndef DUSPINBOX_H
#define DUSPINBOX_H

#include "duwidgets/dulineedit.h"
#include <QSpinBox>

/**
 * @brief The DuSpinBox class Improves QSpinBox
 * @version
 * 1.2.0 Adds autoselect options
 * 1.1.0 Adds edited() signal
 */
class DuSpinBox : public QSpinBox
{
    Q_OBJECT
public:
    DuSpinBox(QWidget *parent = nullptr);

    void setValue(int v);

    bool autoSelect() const { return _lineEdit->autoSelect(); }
    void setAutoSelect(bool newAutoSelect) { _lineEdit->setAutoSelect(newAutoSelect); }

signals:
    /**
     * @brief edited Emitted on editingFinished
     * IF AND ONLY IF the value has actually changed.
     * Emits the new value.
     */
    void edited(int);

private:
    void emitEdited();
    bool _edited = false;
    bool _editing = true;
    DuLineEdit *_lineEdit;
};

#endif // DUSPINBOX_H
