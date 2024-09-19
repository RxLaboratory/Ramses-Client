#include "dudoublespinslider.h"


DuDoubleSpinSlider::DuDoubleSpinSlider(QWidget *parent):
    QStackedWidget( parent )
{
    setupUi();
    connectEvents();

    setDecimals(2);
    setMinimum(0);
    setMaximum(100);
    setValue(0);

    this->setCurrentIndex(1);
}

void DuDoubleSpinSlider::setupUi()
{
    _spinBox = new QDoubleSpinBox(this);
    _spinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);

    this->addWidget(_spinBox);

    _slider = new DuDoubleSlider(this);
    this->addWidget(_slider);

    _spinBox->installEventFilter(this);
}

void DuDoubleSpinSlider::connectEvents()
{
    connect(_slider,SIGNAL(valueChanging(double)), this, SLOT(slider_valueChanging(double)));
    connect(_slider,SIGNAL(editingFinished(double)), this, SLOT(slider_valueChanged(double)));
    connect(_spinBox,SIGNAL(editingFinished()), this, SLOT(spinBox_editingFinished()));
    connect(_spinBox,SIGNAL(valueChanged(double)), this, SLOT(spinBox_valueChanged(double)));
}

QDoubleSpinBox *DuDoubleSpinSlider::spinBox() const
{
    return _spinBox;
}

DuDoubleSlider *DuDoubleSpinSlider::slider() const
{
    return _slider;
}

QString DuDoubleSpinSlider::prefix() const
{
    return _slider->prefix();
}

void DuDoubleSpinSlider::setPrefix(const QString &prefix)
{
    _slider->setPrefix(prefix);
}

QString DuDoubleSpinSlider::suffix() const
{
    return _slider->suffix();
}

void DuDoubleSpinSlider::setSuffix(const QString &suffix)
{
    _slider->setSuffix(suffix);
}

int DuDoubleSpinSlider::decimals() const
{
    return _spinBox->decimals();
}

void DuDoubleSpinSlider::setDecimals(int d)
{
    _spinBox->setDecimals(d);
    _slider->setDecimals(d);
}

int DuDoubleSpinSlider::maximum() const
{
    return _spinBox->maximum();
}

void DuDoubleSpinSlider::setMaximum(double max)
{
    _spinBox->setMaximum(max);
    _slider->setMaximum(max);
}

int DuDoubleSpinSlider::minimum() const
{
    return _spinBox->minimum();
}

void DuDoubleSpinSlider::setMinimum(double min)
{
    _spinBox->setMinimum(min);
    _slider->setMinimum(min);
}

bool DuDoubleSpinSlider::valueVisible() const
{
    return _slider->valueVisible();
}

void DuDoubleSpinSlider::showValue(bool showValue)
{
    _slider->showValue(showValue);
}

double DuDoubleSpinSlider::value() const
{
    return _spinBox->value();
}

void DuDoubleSpinSlider::setValue(double value)
{
    QSignalBlocker b1(_spinBox);
    QSignalBlocker b2(_slider);
    _spinBox->setValue(value);
    _slider->setValue(value);
}

void DuDoubleSpinSlider::spinBox_editingFinished()
{
    this->setCurrentIndex(1);
    _slider->setValue(_spinBox->value());
    emit valueChanged(_spinBox->value());
    emit editingFinished(_spinBox->value());
}

void DuDoubleSpinSlider::spinBox_valueChanged(double arg1)
{
    emit valueChanged(arg1);
}

void DuDoubleSpinSlider::slider_valueChanged(double arg1)
{
    _spinBox->setValue(arg1);
    emit valueChanged(arg1);
    emit editingFinished(_spinBox->value());
}

void DuDoubleSpinSlider::slider_valueChanging(double arg1)
{
    emit valueChanging(arg1);
}

void DuDoubleSpinSlider::mouseReleaseEvent(QMouseEvent *event)
{
    if (this->currentIndex() == 0)
    {
        event->ignore();
        return;
    }
    if (event->button() != Qt::LeftButton)
    {
        event->ignore();
        return;
    }
    if (_originalMousePos != event->pos())
    {
        event->ignore();
        return;
    }
    this->setCurrentIndex(0);
    _spinBox->selectAll();
    _spinBox->setFocus();
    event->accept();
}

void DuDoubleSpinSlider::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        _originalMousePos = event->pos();
    }

    if (event->y() < 0 || event->y() > height() || event->x() < 0 || event->x() > width())
    {
        this->setCurrentIndex(1);
        event->accept();
        return;
    }

    event->ignore();
}

void DuDoubleSpinSlider::focusOutEvent(QFocusEvent *event)
{
    this->setCurrentIndex(1);
    event->accept();
}

bool DuDoubleSpinSlider::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::FocusOut)
    {
        this->setCurrentIndex(1);
        return true;
    }
    else
    {
        // standard event processing
        return QObject::eventFilter(obj, event);
    }
}
