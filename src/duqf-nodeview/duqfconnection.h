#ifndef DUQFCONNECTION_H
#define DUQFCONNECTION_H

#include <QGraphicsObject>
#include <QPainter>

#include "duqf-app/app-style.h"

class DuQFConnection : public QGraphicsObject
{
    Q_OBJECT
public:
    DuQFConnection(QPointF from);

    QRectF boundingRect() const Q_DECL_OVERRIDE;
    void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = nullptr) Q_DECL_OVERRIDE;

public slots:
    void setTo(const QPointF &to);
    void setFrom(const QPointF &from);

private:
    QPointF m_from;
    QPointF m_to;
    qreal m_width;
};

#endif // DUQFCONNECTION_H
