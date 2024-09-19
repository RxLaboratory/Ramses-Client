#ifndef DUDOUBLESPINBOX_H
#define DUDOUBLESPINBOX_H

#include "duwidgets/dulineedit.h"
#include <QDoubleSpinBox>

/**
 * @brief The DuDoubleSpinBox class Improves QDoubleSpinBox
 * @version
 * 1.2.0 Adds autoselect options
 * 1.1.0 Adds edited() signal
 */
class DuDoubleSpinBox : public QDoubleSpinBox
{
    Q_OBJECT
public:
    DuDoubleSpinBox(QWidget *parent = nullptr);

    void setValue(double d);

    bool autoSelect() const { return _lineEdit->autoSelect(); }
    void setAutoSelect(bool newAutoSelect) { _lineEdit->setAutoSelect(newAutoSelect); }

signals:
    /**
     * @brief edited Emitted on editingFinished
     * IF AND ONLY IF the value has actually changed.
     * Emits the new value.
     */
    void edited(qreal);

private:
    void emitEdited();
    bool _edited = false;
    bool _editing = true;
    DuLineEdit *_lineEdit;
};

#endif // DUDOUBLESPINBOX_H
