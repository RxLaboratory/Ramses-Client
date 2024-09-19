#ifndef DULINEEDIT_H
#define DULINEEDIT_H

#include <QLineEdit>
#include <QFocusEvent>
#include <QMouseEvent>
#include <QtDebug>

class DuLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    DuLineEdit(QWidget *parent = nullptr);

    bool autoSelect() const;
    void setAutoSelect(bool newAutoSelect);

signals:
    /**
     * @brief edited Emitted on editingFinished
     * IF AND ONLY IF the value has actually changed.
     * Emits the new value.
     */
    void edited(QString);

protected:
    void focusInEvent(QFocusEvent *event);
    void mousePressEvent(QMouseEvent *me);
private:
    void emitEdited();
    bool _edited = false;

    bool _selectOnMousePress;
    bool _autoSelect = true;
};

#endif // DULINEEDIT_H
