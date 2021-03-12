#include "duqfconnector.h"

DuQFConnector::DuQFConnector(Type type, QColor color)
{
    int s = DuUI::getSize("padding", "small") * 2;
    m_size = QSizeF(s,s);

    if (color.isValid()) m_color = color;
    else m_color = DuUI::getColor("dark-grey");

    m_type = type;

    m_boundingRect = QRectF(-m_size.width() / 2, -m_size.height() / 2, m_size.width(), m_size.height());

    setAcceptHoverEvents(true);
}

QRectF DuQFConnector::boundingRect() const
{
    return m_boundingRect;
}

void DuQFConnector::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget)
    Q_UNUSED(option)

    painter->save();

    // Background
    if (m_hover || m_connecting) painter->setBrush(QBrush(m_color));
    else painter->setBrush(QBrush(m_color.lighter(50)));
    QPen pen( DuUI::getColor("very-dark-grey") );
    pen.setWidth(1);
    painter->setPen(pen);
    painter->drawEllipse(m_boundingRect);

    painter->restore();
}

void DuQFConnector::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        emit connectionInitiated(this->scenePos());
        m_connecting = true;
        event->accept();
        return;
    }

    QGraphicsObject::mousePressEvent(event);
    event->ignore();
}

void DuQFConnector::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (m_connecting)
    {
        emit connectionMoved(event->scenePos());
        event->accept();
        return;
    }

    QGraphicsObject::mousePressEvent(event);
    event->ignore();
}

void DuQFConnector::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (m_connecting)
    {
        emit connectionFinished(event->scenePos(), this->scenePos());
        event->accept();
        m_connecting = false;
        return;
    }

    QGraphicsObject::mousePressEvent(event);
    event->ignore();
}

void DuQFConnector::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
    m_hover = true;
    update();
}

void DuQFConnector::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
    m_hover = false;
    update();
}

DuQFConnector::Type DuQFConnector::connectorType() const
{
    return m_type;
}
