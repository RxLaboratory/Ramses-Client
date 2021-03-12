#include "duqfnode.h"

DuQFNode::DuQFNode(QString title)
{
    setAcceptHoverEvents(true);

    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect();
    shadow->setBlurRadius(5.0);
    shadow->setColor(DuUI::getColor("obsidian"));
    shadow->setOffset(3.0,3.0);
    setGraphicsEffect(shadow);

    m_cornerRadius = DuUI::getSize( "padding", "small" );

    setFlags(ItemIsMovable | ItemIsSelectable | ItemIsFocusable);
    setSelected(false);

    // Connectors

    m_defaultInputConnector = new DuQFConnector( DuQFConnector::Input, DuUI::getColor("light-blue") );
    m_defaultInputConnector->setParentItem(this);
    m_defaultOutputConnector = new DuQFConnector( DuQFConnector::Output, DuUI::getColor("light-green") );
    m_defaultOutputConnector->setParentItem(this);

    // Title

    m_titleItem = new QGraphicsTextItem(title);
    QFont f = qApp->font();
    f.setPixelSize( DuUI::getSize("font", "size-small") );
    m_titleItem->setFont(f);
    m_titleItem->setDefaultTextColor(DuUI::getColor("light-grey"));
    m_titleItem->setParentItem(this);
    m_padding = DuUI::getSize("padding", "small");
    //m_titleItem->setPos(m_padding, m_padding);

    setTitle(title);

    connect(m_defaultInputConnector, &DuQFConnector::connectionInitiated, this, &DuQFNode::connectionInitiated);
    connect(m_defaultInputConnector, &DuQFConnector::connectionMoved, this, &DuQFNode::connectionMoved);
    connect(m_defaultInputConnector, &DuQFConnector::connectionFinished, this, &DuQFNode::connectionFinished);
    connect(m_defaultOutputConnector, &DuQFConnector::connectionInitiated, this, &DuQFNode::connectionInitiated);
    connect(m_defaultOutputConnector, &DuQFConnector::connectionMoved, this, &DuQFNode::connectionMoved);
    connect(m_defaultOutputConnector, &DuQFConnector::connectionFinished, this, &DuQFNode::connectionFinished);
}

DuQFNode::DuQFNode(const DuQFNode &other)
{
    setTitle(other.title());
}

DuQFNode::~DuQFNode()
{

}

QRectF DuQFNode::boundingRect() const
{
    return m_boundingRect;
}

void DuQFNode::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget)
    Q_UNUSED(option)

    painter->save();

    // Background

    QPainterPath path;
    //const QRectF rect(-m_size.width() / 2, -m_size.height() / 2, m_size.width(), m_size.height());
    const QRectF rect = boundingRect();
    path.addRoundedRect(rect, m_cornerRadius, m_cornerRadius);
    painter->setRenderHint(QPainter::Antialiasing);

    const QBrush brush( DuUI::getColor("medium-grey") );
    painter->fillPath(path, brush);

    // Selection Stroke

    if (isSelected())
    {
        QPen pen( DuUI::getColor("less-light-grey") );
        pen.setWidth(2);
        painter->strokePath(path, pen);
    }

    // Inputs

    painter->restore();
}

void DuQFNode::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        setSelected(true);
        event->accept();
        return;
    }

    QGraphicsItem::mousePressEvent(event);
    event->ignore();
}

QString DuQFNode::title() const
{
    return m_titleItem->toPlainText();
}

void DuQFNode::setTitle(const QString &title)
{
    m_titleItem->setPlainText(title);
    m_boundingRect = m_titleItem->boundingRect().adjusted(-m_padding-7, -m_padding-7, m_padding+7, m_padding+7);
    prepareGeometryChange();

    m_defaultOutputConnector->setPos(m_boundingRect.right(), m_boundingRect.center().y());
    m_defaultInputConnector->setPos(m_boundingRect.left() , m_boundingRect.center().y());
}
