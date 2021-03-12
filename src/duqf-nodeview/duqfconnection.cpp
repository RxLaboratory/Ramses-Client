#include "duqfconnection.h"

DuQFConnection::DuQFConnection(QPointF from)
{
    m_from = from;
    m_width = DuUI::getSize("margin");
    setZValue(-1000);
}

QRectF DuQFConnection::boundingRect() const
{

    qreal x;
    qreal y;
    qreal w;
    qreal h;

    if ( m_from.x() < m_to.x() )
    {
        x = m_from.x();
        w = m_to.x() - m_from.x();
    }
    else
    {
        x = m_to.x();
        w = m_from.x() - m_to.x();
    }

    if ( m_from.y() < m_to.y() )
    {
        y = m_from.y();
        h = m_to.y() - m_from.y();
    }
    else
    {
        y = m_to.y();
        h = m_from.y() - m_to.y();
    }

    return QRectF(x, y, w, h).adjusted(-m_width, -m_width, m_width, m_width);
}

void DuQFConnection::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget)
    Q_UNUSED(option)

    painter->save();

    QPainterPath path( QPointF( m_from.x(), m_from.y() ) );

    path.cubicTo( (m_from.x()*2 + m_to.x())/3, m_from.y(), (m_from.x() + m_to.x()*2)/3, m_to.y(), m_to.x(), m_to.y());

    painter->setBrush(QBrush(Qt::transparent));
    QPen pen( DuUI::getColor("less-light-grey") );
    pen.setWidth( m_width );
    pen.setCapStyle(Qt::RoundCap);
    painter->setPen(pen);

    painter->drawPath(path);

    painter->restore();
}

void DuQFConnection::setTo(const QPointF &to)
{
    m_to = to;
    prepareGeometryChange();
}

void DuQFConnection::setFrom(const QPointF &from)
{
    m_from = from;
    prepareGeometryChange();
}
