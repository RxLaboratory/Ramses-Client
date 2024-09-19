#ifndef DUDOUBLESPINSLIDER_H
#define DUDOUBLESPINSLIDER_H

#include <QStackedWidget>
#include <QDoubleSpinBox>

#include "dudoubleslider.h"

class DuDoubleSpinSlider : public QStackedWidget
{
    Q_OBJECT
public:
    DuDoubleSpinSlider(QWidget *parent = nullptr);

    QString suffix() const;
    void setSuffix(const QString &suffix);

    QString prefix() const;
    void setPrefix(const QString &prefix);

    int decimals() const;
    void setDecimals(int d);

    int minimum() const;
    void setMinimum(double min);

    int maximum() const;
    void setMaximum(double max);

    double value() const;
    void setValue(double value);

    bool valueVisible() const;
    void showValue(bool showValue);

    DuDoubleSlider *slider() const;
    QDoubleSpinBox *spinBox() const;

signals:
    void valueChanged(double value);
    void valueChanging(double value);
    void editingFinished(double value); // Emitted by user interaction only

private slots:
    void spinBox_editingFinished();
    void spinBox_valueChanged(double arg1);
    void slider_valueChanged(double arg1);
    void slider_valueChanging(double arg1);
private:
    void setupUi();
    void connectEvents();
    DuDoubleSlider *_slider;
    QDoubleSpinBox *_spinBox;
    QPoint _originalMousePos;
protected:
    void mouseReleaseEvent ( QMouseEvent * event );
    void mousePressEvent ( QMouseEvent * event );
    void focusOutEvent(QFocusEvent* event);
    bool eventFilter(QObject *obj, QEvent *event);
};

#endif // DUDOUBLESPINSLIDER_H
