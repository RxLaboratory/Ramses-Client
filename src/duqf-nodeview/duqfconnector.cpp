#include "duqfconnector.h"

DuQFConnector::DuQFConnector(QPointF from)
{
    m_from = from;
    m_width = DuUI::getSize("margin");
    setZValue(-1000);

    setFlags(ItemIsSelectable);
}

QRectF DuQFConnector::boundingRect() const
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

QPainterPath DuQFConnector::shape() const
{
    QPainterPath path;
    QPolygonF polygon;
    polygon << m_from;
    polygon << m_fromHandle;
    polygon << m_to;
    polygon << m_toHandle;
    path.addPolygon(polygon);

    return path;
}

void DuQFConnector::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget)
    Q_UNUSED(option)

    painter->save();

    // Get handle coordinates
    float handleWeight = QSettings().value("nodeViews/curvature", 0.5).toFloat();
    m_fromHandle.setX((m_from.x()*(1.0-handleWeight) + m_to.x()*handleWeight));
    m_fromHandle.setY(m_from.y());
    m_toHandle.setX((m_from.x()*handleWeight + m_to.x()*(1.0-handleWeight)));
    m_toHandle.setY(m_to.y());

    // Create path
    QPainterPath path( QPointF( m_from.x(), m_from.y() ) );
    path.cubicTo( m_fromHandle, m_toHandle, m_to);

    // And draw
    painter->setBrush(QBrush(Qt::transparent));
    QPen pen;
    if (isSelected()) pen.setColor( DuUI::getColor("light-grey") );
    else pen.setColor( DuUI::getColor("less-light-grey") );
    pen.setWidth( m_width );
    pen.setCapStyle(Qt::RoundCap);
    painter->setPen(pen);

    painter->drawPath(path);

    painter->restore();
}

void DuQFConnector::setTo(const QPointF &to)
{
    m_to = to;
    prepareGeometryChange();
}

void DuQFConnector::setFrom(const QPointF &from)
{
    m_from = from;
    prepareGeometryChange();
}
