#include "duqfnode.h"

DuQFNode::DuQFNode(QString title)
{
    setAcceptHoverEvents(true);
    m_grid = nullptr;

    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect();
    shadow->setBlurRadius(10.0);
    shadow->setColor(DuUI::getColor("obsidian"));
    shadow->setOffset(5.0,5.0);
    setGraphicsEffect(shadow);

    m_cornerRadius = DuUI::getSize( "padding", "small" );

    setFlags(ItemIsMovable | ItemIsSelectable | ItemIsFocusable | ItemSendsGeometryChanges);
    setSelected(false);

    // Connectors

    m_defaultInputConnector = new DuQFSlot( DuQFSlot::Input, false, DuUI::getColor("light-blue") );
    m_defaultInputConnector->setParentItem(this);
    m_defaultOutputConnector = new DuQFSlot( DuQFSlot::Output, false, DuUI::getColor("light-green") );
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

    connect(m_defaultInputConnector, &DuQFSlot::connectionInitiated, this, &DuQFNode::connectionInitiated);
    connect(m_defaultInputConnector, &DuQFSlot::connectionMoved, this, &DuQFNode::connectionMoved);
    connect(m_defaultInputConnector, &DuQFSlot::connectionFinished, this, &DuQFNode::connectionFinished);
    connect(m_defaultOutputConnector, &DuQFSlot::connectionInitiated, this, &DuQFNode::connectionInitiated);
    connect(m_defaultOutputConnector, &DuQFSlot::connectionMoved, this, &DuQFNode::connectionMoved);
    connect(m_defaultOutputConnector, &DuQFSlot::connectionFinished, this, &DuQFNode::connectionFinished);
}

DuQFNode::DuQFNode(const DuQFNode &other)
{
    setTitle(other.title());
    setGrid(other.grid());
}

DuQFNode::~DuQFNode()
{

}

QRectF DuQFNode::boundingRect() const
{
    return m_titleItem->boundingRect().adjusted(-m_padding-7, -m_padding-3, m_padding+7, m_padding+3);
}

void DuQFNode::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget)
    Q_UNUSED(option)

    painter->save();

    // Background

    QPainterPath path;
    //const QRectF rect(-m_size.width() / 2, -m_size.height() / 2, m_size.width(), m_size.height());
    const QRectF rect = boundingRect().adjusted(2,2,-2,-2);
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

QVariant DuQFNode::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionChange && scene() && m_grid)
            return m_grid->snapToGrid(value.toPointF());

    return QGraphicsItem::itemChange(change, value);
}

QList<DuQFNode *> DuQFNode::parentNodes() const
{
    return m_parentNodes;
}

bool DuQFNode::isOrphan() const
{
    return !hasChildren() && !hasParents();
}

bool DuQFNode::hasChildren() const
{
    return m_childrenNodes.count() != 0;
}

bool DuQFNode::hasParents() const
{
    return m_parentNodes.count() != 0;
}

QList<DuQFNode *> DuQFNode::childrenNodes() const
{
    return m_childrenNodes;
}

DuQFGrid *DuQFNode::grid() const
{
    return m_grid;
}

void DuQFNode::addChildNode(DuQFNode *childNode)
{
    m_childrenNodes << childNode;
}

void DuQFNode::addParentNode(DuQFNode *parentNode)
{
    m_parentNodes << parentNode;
}

void DuQFNode::removeChildNode(DuQFNode *childNode)
{
    m_childrenNodes.removeAll(childNode);
}

void DuQFNode::removeParentNode(DuQFNode *parentNode)
{
    m_parentNodes.removeAll(parentNode);
}

void DuQFNode::setGrid(DuQFGrid *grid)
{
    m_grid = grid;
}

QString DuQFNode::title() const
{
    return m_titleItem->toPlainText();
}

void DuQFNode::setTitle(const QString &title)
{
    m_titleItem->setPlainText(title);
    QRectF rect = m_titleItem->boundingRect().adjusted(-m_padding-7, -m_padding-3, m_padding+7, m_padding+3);
    m_defaultOutputConnector->setPos(rect.right() , rect.center().y());
    m_defaultInputConnector->setPos(rect.left(), rect.center().y());
}

void DuQFNode::remove()
{
    if (m_removing) return;
    m_removing = true;
    m_defaultInputConnector->remove();
    m_defaultOutputConnector->remove();
    emit removed();
    deleteLater();
}
