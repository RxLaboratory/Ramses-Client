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

protected:
    void focusInEvent(QFocusEvent *event);
    void mousePressEvent(QMouseEvent *me);
private:
    bool _selectOnMousePress;

    bool _autoSelect = true;
};

#endif // DULINEEDIT_H
