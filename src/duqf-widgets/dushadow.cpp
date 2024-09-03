#include "duqf-widgets/dushadow.h"
#include "qgraphicsitem.h"
#include <QGraphicsScene>

Q_DECL_IMPORT void qt_blurImage(QPainter *p, QImage &blurImage, qreal radius, bool quality, bool alphaOnly, int transposed = 0); // src/widgets/effects/qpixmapfilter.cpp

DuShadow::DuShadow(QObject *parent) :QGraphicsEffect(parent) {

}

DuShadow::DuShadow(const QColor &c, qreal distance, qreal radius, Sides s, QObject *parent) : QGraphicsEffect(parent) {
    setColor(c);
    setBlurRadius(radius);
    setDistance(distance);
    setSides(s);
}

QRectF DuShadow::boundingRectFor(const QRectF &r) const {
    qreal _delta = blurRadius() + distance();
    return r.marginsAdded(QMarginsF(
        (sides() & Side::Left) ? _delta : 0,
        (sides() & Side::Top) ? _delta : 0,
        (sides() & Side::Right) ? _delta : 0,
        (sides() & Side::Bottom) ? _delta : 0
        ));
}

void DuShadow::draw(QPainter *painter) {
    if ((blurRadius() + distance()) <= 0) {
        drawSource(painter);
        return;
    }

    QPoint _offset;
    QPixmap _pixmap = sourcePixmap(Qt::DeviceCoordinates, &_offset, QGraphicsEffect::PadToEffectiveBoundingRect);
    if (_pixmap.isNull()) return;

    QTransform _transform = painter->worldTransform();
    painter->setWorldTransform(QTransform());

    QSize _backgroundSize = QSize(_pixmap.size().width() + 2 * distance(), _pixmap.size().height() + 2 * distance());
    QImage _temp(_backgroundSize, QImage::Format_ARGB32_Premultiplied);

    QPixmap scaled = _pixmap.scaled(_backgroundSize);
    _temp.fill(0);

    QPainter _tempPainter(&_temp);
    _tempPainter.setCompositionMode(QPainter::CompositionMode_Source);
    _tempPainter.drawPixmap(QPointF(-distance(), -distance()), scaled);
    _tempPainter.end();

    QImage blurred(_temp.size(), QImage::Format_ARGB32_Premultiplied);
    blurred.fill(0);

    QPainter blurPainter(&blurred);
    qt_blurImage(&blurPainter, _temp, blurRadius(), true, false);
    blurPainter.end();
    _temp = blurred;

    _tempPainter.begin(&_temp);
    _tempPainter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    _tempPainter.fillRect(_temp.rect(), color());
    _tempPainter.end();

    painter->drawImage(_offset, _temp);
    painter->drawPixmap(_offset, _pixmap, QRectF());
    painter->setWorldTransform(_transform);
}

QPixmap DuShadow::grab(QWidget *target, const QRect &rect, int offset) {
    auto result = QPixmap(rect.size());
    auto r = rect.marginsRemoved(QMargins(offset, offset, offset, offset));
    result.fill(Qt::transparent);
    {
        QPainter p;
        p.begin(&result);
        target->render(&p, QPoint(offset, offset), r);
        p.end();
    }
    return result;
}

QImage DuShadow::paint(QWidget *target, const QRect &box, qreal radius, qreal distance, const QColor &c, Sides sides) {
    const auto _source = grab(target, box, distance);
    if (_source.isNull() || distance <= 0) return QImage();

    QImage _backgroundImage(box.size(), QImage::Format_ARGB32_Premultiplied);
    _backgroundImage.fill(0);

    QPainter _backgroundPainter(&_backgroundImage);
    _backgroundPainter.drawPixmap(QPointF(), _source);
    _backgroundPainter.end();

    QImage blurredImage(_backgroundImage.size(), QImage::Format_ARGB32_Premultiplied);
    blurredImage.fill(c);
    QImage alpha(blurredImage.size(), QImage::Format_ARGB32);
    alpha.fill(QColor(100,100,100));
    blurredImage.setAlphaChannel(alpha);

    _backgroundImage = blurImage(blurredImage, radius);

    _backgroundPainter.begin(&_backgroundImage);
    _backgroundPainter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    auto margin = _backgroundImage.rect().marginsRemoved(QMargins(
        (sides & Left) ? 0 : distance,
        (sides & Top) ? 0 : distance,
        (sides & Right) ? 0 : distance,
        (sides & Bottom) ? 0 : distance
        ));
    _backgroundPainter.fillRect(margin, c);
    _backgroundPainter.end();
    return _backgroundImage;
}

QImage DuShadow::blurImage(QImage src, qreal radius)
{
    QGraphicsBlurEffect blur;
    blur.setBlurRadius(radius);
    blur.setBlurHints(QGraphicsBlurEffect::PerformanceHint);

    QGraphicsScene *scene = new QGraphicsScene();
    QGraphicsPixmapItem *item = new QGraphicsPixmapItem(QPixmap::fromImage(src));
    item->setGraphicsEffect(&blur);
    scene->addItem(item);

    QImage res(src.size()+QSize(radius*2, radius*2), QImage::Format_ARGB32);
    res.fill(Qt::transparent);
    QPainter ptr(&res);
    scene->render(&ptr, QRectF(), QRectF( -radius, -radius, src.width()+radius*2, src.height()+radius*2 ) );

    scene->deleteLater();

    return res;
}
