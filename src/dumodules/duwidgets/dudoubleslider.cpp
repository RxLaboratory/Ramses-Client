#include "dudoubleslider.h"

DuDoubleSlider::DuDoubleSlider(QWidget *parent):
    QProgressBar( parent )
{
    _locale = QLocale::system();
    setFormat("");
    setDecimals(2);
    setMaximum(100);
    setMinimum(0);
    setValue(0);
    _showValue = true;
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
}

void DuDoubleSlider::setValue(double v)
{
    _value = v;
    QProgressBar::setValue(v * _multiplicator);
    emit valueChanged(v);
    repaint();
}

double DuDoubleSlider::value() const
{
    return _value;
}

int DuDoubleSlider::decimals() const
{
    return _decimals;
}

void DuDoubleSlider::setDecimals(int d)
{
    _decimals = d;
    _multiplicator = qPow(10, d);
}

double DuDoubleSlider::maximum() const
{
    return double(QProgressBar::maximum()) / double(_multiplicator);
}

void DuDoubleSlider::setMaximum(double m)
{
    QProgressBar::setMaximum(m * _multiplicator);
}

double DuDoubleSlider::minimum() const
{
    return double(QProgressBar::minimum()) / double(_multiplicator);
}

void DuDoubleSlider::setMinimum(double m)
{
    QProgressBar::setMinimum(m * _multiplicator);
}

QString DuDoubleSlider::text() const
{
    if (_value < minimum()) return "";
    if (!_showValue) return _prefix + _suffix;
    QString t = _locale.toString(_value,'f',_decimals);
    return _prefix + t + _suffix;
}

QString DuDoubleSlider::prefix() const
{
    return _prefix;
}

void DuDoubleSlider::setPrefix(const QString &prefix)
{
    _prefix = prefix;
}

QString DuDoubleSlider::suffix() const
{
    return _suffix;
}

void DuDoubleSlider::setSuffix(const QString &suffix)
{
    _suffix = suffix;
}

void DuDoubleSlider::showValue(bool show)
{
    _showValue = show;
}

bool DuDoubleSlider::valueVisible() const
{
    return _showValue;
}

void DuDoubleSlider::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        _editing = true;
    }
    else {
        _editing = false;
    }
    QProgressBar::mousePressEvent(event);
}

void DuDoubleSlider::mouseMoveEvent(QMouseEvent *event)
{
    if (!_editing) {
        QProgressBar::mouseMoveEvent(event);
        return;
    }

    double newVal;
    double min = minimum();
    double max = maximum();
    if (orientation() == Qt::Vertical)
    {
        double h(height());
        double y(event->y());
        newVal = min + ((max-min) * (h - y)) / h;
    }
    else
    {
        double w(width());
        double x(event->x());
        newVal = min + ((max-min) * x) / w;
    }

    if (invertedAppearance() == true)
        newVal = max - newVal;

    newVal = std::fmin(newVal, max);
    newVal = std::fmax(newVal, min);

    event->accept();

    emit valueChanging(newVal);
    setValue(newVal);

    //qApp->processEvents();
}

void DuDoubleSlider::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton || !_editing) {
        QProgressBar::mouseReleaseEvent(event);
        return;
    }
    emit editingFinished(this->value());
    _editing = false;
    event->ignore();
}
