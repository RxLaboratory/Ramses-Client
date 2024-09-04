#include "duscrollbar.h"

DuScrollBar::DuScrollBar(Qt::Orientation orientation, QWidget *parent):
    QScrollBar(orientation, parent)
{
    setupUi();
    connectEvents();
}

DuScrollBar::DuScrollBar(QWidget *parent):
    QScrollBar(parent)
{
    setupUi();
    connectEvents();
}

QSize DuScrollBar::sizeHint() const
{
    if (orientation() == Qt::Vertical)
        return QSize(m_width, 1);
    else
        return QSize(1, m_width);
}

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
void DuScrollBar::enterEvent(QEvent *)
#else
void DuScrollBar::enterEvent(QEnterEvent *)
#endif
{
    m_sizeAnimation.setDirection(QAbstractAnimation::Forward);
    m_sizeAnimation.start();
}

void DuScrollBar::leaveEvent(QEvent *)
{
    m_sizeAnimation.setDirection(QAbstractAnimation::Backward);
    m_sizeAnimation.start();
}

void DuScrollBar::changeWidth(QVariant width)
{
    m_width = width.toInt();
    if (this->orientation() == Qt::Vertical)
        setFixedWidth(m_width);
    else
        setFixedHeight(m_width);
}

void DuScrollBar::setupUi()
{
    m_sizeAnimation.setDuration(100);
    m_sizeAnimation.setStartValue(3);
    m_sizeAnimation.setEndValue(10);
    m_sizeAnimation.setEasingCurve(
        QEasingCurve( QEasingCurve::InOutCubic )
        );

    changeWidth(m_sizeAnimation.startValue());
}

void DuScrollBar::connectEvents()
{
    connect(&m_sizeAnimation, &QVariantAnimation::valueChanged, this, &DuScrollBar::changeWidth);
}
