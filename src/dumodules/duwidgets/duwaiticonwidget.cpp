#include "duwaiticonwidget.h"
#include "duwidgets/dusvgiconengine.h"
#include "qpainter.h"
#include "duwidgets/duicon.h"

DuWaitIconWidget::DuWaitIconWidget(double speed, double fps, QWidget *parent)
    : QWidget{parent},
    _size(QSize(16,16)),
    _speed(speed)
{
    auto icon = new DuSVGIconEngine(":/icons/wait");
    icon->setMainColor( icon->checkedColor() );
    _icon = DuIcon(icon);
    _timer.setInterval(1000.0/fps);
    setFixedSize(_size);
    connect( &_timer, &QTimer::timeout, this, [this] { update(); });
}

void DuWaitIconWidget::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.setRenderHint(QPainter::HighQualityAntialiasing);

    int time = _elapsed.elapsed();
    // Default speed in 360 per 4000 ms, i.e. .09
    double angle = time*_speed*.09;

    int tw = _size.width()/2;
    painter.translate(tw, tw);
    painter.rotate(angle);
    painter.translate(-tw, -tw);
    QPixmap pix = _icon.pixmap(_size);
    painter.drawPixmap(0,0, pix);
}
