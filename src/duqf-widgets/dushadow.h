#ifndef DUSHADOW_H
#define DUSHADOW_H

#pragma once
#include <QGraphicsEffect>
#include <QPainter>
#include <QWidget>

class DuShadow : public QGraphicsEffect {
public:
    enum Side {
        Left = 0x1,
        Right = 0x2,
        Bottom = 0x4,
        Top = 0x8,
        Around = Left | Top | Right | Bottom,
    };
    Q_DECLARE_FLAGS(Sides, Side);

    DuShadow(QObject *parent = 0);
    DuShadow(const QColor &c, qreal distance, qreal radius, Sides sides = Side::Around, QObject *parent = 0);

    Sides sides() const {
        return _side;
    }
    void setSides(Sides s) {
        _side = s;
        updateBoundingRect();
    }

    QColor color() const {
        return _color;
    }
    void setColor(const QColor &c) {
        _color = c;
        updateBoundingRect();
    }

    qreal blurRadius() const {
        return _blurRadius;
    }
    void setBlurRadius(qreal br) {
        _blurRadius = br;
        updateBoundingRect();
    }

    qreal distance() const {
        return _distance;
    }
    void setDistance(qreal d) {
        _distance = d;
        updateBoundingRect();
    }

    QRectF boundingRectFor(const QRectF &) const override;

    static QPixmap grab(QWidget *target, const QRect &rect, int offset);  // Return a pixmap with target painted into it with margin = offset

    // Return a background blurred QImage to Draw as the widget's shadow
    static QImage paint(QWidget *target, const QRect &box, qreal radius, qreal distance, const QColor &c, Sides sides = Side::Around);
    static QImage blurImage(QImage src, qreal radius);

protected:
    void draw(QPainter *painter) override;

private:
    Sides _side;
    QColor _color;
    qreal _distance;
    qreal _blurRadius;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(DuShadow::Sides)

#endif // DUSHADOW_H
