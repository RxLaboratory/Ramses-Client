#include "duiconwidget.h"

#include <QPaintEvent>

#include "duapp/dusettings.h"
#include "duwidgets/dusvgiconengine.h"
#include "qpainter.h"
#include "duwidgets/duicon.h"

DuIconWidget::DuIconWidget(QWidget *parent)
    : QWidget{parent}
{
    setFixedSize(_size);
    _color = DuSettings::i()->get(DuSettings::UI_IconColor).value<QColor>();
    _timer.setInterval(1000.0/24.0);
    connect( &_timer, &QTimer::timeout, this, [this] { update(); });
}

void DuIconWidget::setSize(const QSize &s)
{
    if (_size == s)
        return;

    _size = s;
    setFixedSize(s);
    if (!_icon.isNull())
        _pix = _icon.pixmap(_size);

    update();
}

void DuIconWidget::rotate(double speed, double fps, bool ccw)
{
    if (ccw && _animation == RotateCCW)
        return;
    if (_animation == RotateCW)
        return;

    _speed = speed;
    _timer.setInterval(1000.0/fps);

    if (ccw)
        _animation = RotateCCW;
    else
        _animation = RotateCW;

    _timer.start();
    _elapsed.start();

    update();
}

void DuIconWidget::stopAnimation()
{
    _animation = None;
    _timer.stop();

    update();
}

void DuIconWidget::showEvent(QShowEvent *)
{
    if (_animation != None) {
        _timer.start();
        _elapsed.start();
    }
}

void DuIconWidget::hideEvent(QHideEvent *)
{
    _timer.stop();
}

void DuIconWidget::setSVGIcon(const QString &iconPath)
{
    if (_iconPath == iconPath)
        return;

    _iconPath = iconPath;
    updateIcon();
}

void DuIconWidget::setColor(const QColor &color)
{
    if(_color == color)
        return;
    _color = color;
    updateIcon();
}

void DuIconWidget::paintEvent(QPaintEvent *e)
{
    e->accept();

    if (_pix.isNull())
        return;

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.setRenderHint(QPainter::HighQualityAntialiasing);

    if (_animation != None) {

        int time = _elapsed.elapsed();
        // Default speed in 360 per 4000 ms, i.e. .09
        double angle = time*_speed*.09;
        if (_animation == RotateCCW)
            angle = -angle;

        int tw = _size.width()/2;
        painter.translate(tw, tw);
        painter.rotate(angle);
        painter.translate(-tw, -tw);
    }

    painter.drawPixmap(0,0, _pix);
}

void DuIconWidget::updateIcon()
{
    if (_iconPath == "") {
        _icon = DuIcon();
    }
    else {
        auto icon = new DuSVGIconEngine(_iconPath);
        icon->setMainColor( _color );
        _icon = DuIcon(icon);
    }

    updatePixmap();
}

void DuIconWidget::updatePixmap()
{
    if (_icon.isNull())
        _pix = QPixmap();
    else
        _pix = _icon.pixmap(_size);
    update();
}
