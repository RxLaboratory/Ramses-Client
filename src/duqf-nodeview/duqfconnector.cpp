#include "duqfconnector.h"
#include "duqf-app/dusettingsmanager.h"

DuQFConnector::DuQFConnector(QString title, QGraphicsItem *parent):
    QGraphicsObject( parent )
{
    setupUi();

    m_from = QPointF();
    m_to = m_from;
    m_width = 3;
    setZValue(-1000);

    setFlags(ItemIsSelectable);

    m_titleItem->setPlainText(title);

    m_toColor = QColor(227,227,227);
    m_fromColor = QColor(227,227,227);
}

DuQFConnector::DuQFConnector(QPointF from, QString title)
{
    setupUi();

    m_from = from;
    m_to = m_from;
    m_width = 3;
    setZValue(-1000);

    setFlags(ItemIsSelectable);

    m_titleItem->setPlainText(title);

    m_toColor = QColor(227,227,227);
    m_fromColor = QColor(227,227,227);
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

    return QRectF(x, y, w, h).adjusted(-m_width - m_titleItem->boundingRect().width(), -m_width - m_titleItem->boundingRect().height(), m_width + m_titleItem->boundingRect().width(), m_width + m_titleItem->boundingRect().height());
}

QPainterPath DuQFConnector::shape() const
{
    QPainterPath path;
    QRectF rect = QRectF(m_from, m_to);

    if (std::abs(rect.height()) > 10 && std::abs(rect.width()) > 10)
    {
        CurveHandles handles = curveHandles(.5);
        QPolygonF polygon;
        polygon << m_from;
        polygon << handles.from;
        polygon << m_to;
        polygon << handles.to;
        polygon << m_from;
        path.addPolygon(polygon);
    }
    else
    {
        // Adjustments
        int dx1, dy1, dx2, dy2;
        if (rect.height() > 0) {
            dy1 = -5;
            dy2 = 5;
        }
        else {
            dy1 = 5;
            dy2 = -5;
        }
        if (rect.width() > 0) {
            dx1 = -5;
            dx2 = 5;
        }
        else {
            dx1 = 5;
            dx2 = -5;
        }
        path.addRect( rect.adjusted( dx1,dy1,dx2,dy2));
    }

    return path;
}

void DuQFConnector::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget)
    Q_UNUSED(option)

    painter->save();

    QPainterPath path;

    // Create path
    if (DuSettingsManager::instance()->nvCurvature() < 1) {
        CurveHandles handles = curveHandles();
        path = QPainterPath(( QPointF( m_from.x(), m_from.y() ) ));
        path.cubicTo( handles.from, handles.to, m_to);
    }
    else {
        QPolygonF poly;
        poly.append(m_from);
        poly.append(QPointF((m_from.x() + m_to.x()) / 2, m_from.y()));
        poly.append(QPointF((m_from.x() + m_to.x()) / 2, m_to.y()));
        poly.append(m_to);
        path.addPolygon(poly);
    }


    // And draw
    painter->setBrush(QBrush(Qt::transparent));

    QLinearGradient gradient(m_from, m_to);

    if (isSelected()) {
        gradient.setColorAt(0.0, m_fromColor);
        gradient.setColorAt(0.5, QColor(227,227,227));
        gradient.setColorAt(1.0, m_toColor);
    }
    else {
        gradient.setColorAt(0.0, m_fromColor.darker());
        //gradient.setColorAt(0.5, DuUI::getColor("less-light-grey"));
        gradient.setColorAt(1.0, m_toColor.darker());
    }
    QPen pen(gradient, m_width, Qt::SolidLine, Qt::RoundCap);
    painter->setPen(pen);

    painter->drawPath(path);

#ifdef QT_DEBUG
    // Let's draw the selection box
    QPainterPath selPath = shape();
    pen.setWidth( m_width/3 );
    painter->setPen(pen);
    painter->drawPath(selPath);
#endif

    // Title Background
    if (title() != "")
    {
        QPainterPath path;
        const QRectF rect(m_titleItem->pos().x(),
                          m_titleItem->pos().y(),
                          m_titleItem->boundingRect().width() +2,
                          m_titleItem->boundingRect().height() + 2);
        path.addRoundedRect(rect, m_cornerRadius, m_cornerRadius);
         painter->setRenderHint(QPainter::Antialiasing);

         const QBrush brush( QColor(227,227,227) );
         painter->fillPath(path, brush);

         // Selection Stroke

         if (isSelected())
         {
             QPen pen( QColor(227,227,227) );
             pen.setWidth(2);
             painter->strokePath(path, pen);
         }
    }



    painter->restore();
}

QString DuQFConnector::title() const
{
    return m_titleItem->toPlainText();
}

void DuQFConnector::setTitle(const QString &title)
{
    m_titleItem->setPlainText(title);
    updateTitlePos();
}

void DuQFConnector::setTo(const QPointF &to)
{
    m_to = to;
    updateTitlePos();
    prepareGeometryChange();
}

void DuQFConnector::setFrom(const QPointF &from)
{
    m_from = from;
    updateTitlePos();
    prepareGeometryChange();
}

void DuQFConnector::remove()
{
    if (m_removing) return;
    m_removing = true;
    emit removed();
    deleteLater();
}

QVariant DuQFConnector::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemSelectedChange && scene())
        emit selected(value.toBool());

    return QGraphicsItem::itemChange(change, value);
}

void DuQFConnector::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    emit clicked();
    QGraphicsObject::mouseReleaseEvent(event);
}

void DuQFConnector::setupUi()
{
    m_titleItem = new QGraphicsTextItem("");
    QFont f = qApp->font();
    f.setPixelSize( 10 );
    m_titleItem->setFont(f);
    m_titleItem->setDefaultTextColor( QColor(51,51,51) );
    m_titleItem->setParentItem(this);
    m_padding = 5;
}

void DuQFConnector::updateTitlePos()
{
    QPointF p = (m_from + m_to) / 2;
    // Adjust with text size
    p = p - m_titleItem->boundingRect().center();
    m_titleItem->setPos(p);
    //m_titleItem->setPos(boundingRect().width() / 2 - m_titleItem->boundingRect().width() / 2 + pos().x(),
    //                    boundingRect().height() / 2 - m_titleItem->boundingRect().height() / 2 + pos().y());
}

void DuQFConnector::setToColor(const QColor &newToColor)
{
    m_toColor = newToColor;
}

void DuQFConnector::setFromColor(const QColor &newFromColor)
{
    m_fromColor = newFromColor;
}

CurveHandles DuQFConnector::curveHandles(float q) const
{
    // Get handle coordinates
    float handleWeight = DuSettingsManager::instance()->nvCurvature();
    handleWeight *= q;
    CurveHandles handles;
    handles.from.setX((m_from.x()*(1.0-handleWeight) + m_to.x()*handleWeight));
    handles.from.setY(m_from.y());
    handles.to.setX((m_from.x()*handleWeight + m_to.x()*(1.0-handleWeight)));
    handles.to.setY(m_to.y());
    return handles;
}
