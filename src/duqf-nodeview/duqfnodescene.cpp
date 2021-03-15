#include "duqfnodescene.h"

DuQFNodeScene::DuQFNodeScene()
{
    setSceneRect(-m_initialSize, -m_initialSize, m_initialSize * 2, m_initialSize * 2);
}

void DuQFNodeScene::adjustSceneRect()
{
    while (!containsAll()) {
        setSceneRect(sceneRect().adjusted(-m_initialSize, -m_initialSize, m_initialSize, m_initialSize));
    }
}

QRectF DuQFNodeScene::zoomToFit(bool isForExport) const
{
    return calculateRectangle(isForExport);
}

void DuQFNodeScene::addNode()
{
    // remove selection
    foreach(QGraphicsItem *i, items()) i->setSelected(false);

    DuQFNode *node = new DuQFNode();
    this->addItem(node);
    node->setSelected(true);

    connect(node, &DuQFNode::connectionInitiated, this, &DuQFNodeScene::initiateConnection);
    connect(node, &DuQFNode::connectionMoved, this, &DuQFNodeScene::moveConnection);
    connect(node, &DuQFNode::connectionFinished, this, &DuQFNodeScene::finishConnection);
}

void DuQFNodeScene::removeSelectedConnections()
{
    foreach(QGraphicsItem *item, selectedItems())
    {
        DuQFConnector *c = qgraphicsitem_cast<DuQFConnector*>(item);
        if (c) c->remove();
    }
}

void DuQFNodeScene::removeSelectedNodes()
{
    foreach(QGraphicsItem *item, selectedItems())
    {
        DuQFNode *n = qgraphicsitem_cast<DuQFNode*>(item);
        if (n) n->remove();
    }
}

void DuQFNodeScene::moveConnection(QPointF to)
{
    // Snap
    // Check the item we're connecting to
    foreach(QGraphicsItem *item, items(to))
    {
        DuQFSlot *input = qgraphicsitem_cast<DuQFSlot*>(item);
        if(input)
        {
            to = input->scenePos();
            break;
        }
    }

    if (m_connecting && m_connectingItem)
    {
        m_connectingItem->setTo(to);
    }
}

void DuQFNodeScene::finishConnection(QPointF to, QPointF from)
{
    m_connecting = false;

    // Check the item we're connecting to
    DuQFSlot *input = nullptr;
    foreach(QGraphicsItem *item, items(to))
    {
        input = qgraphicsitem_cast<DuQFSlot*>(item);
        if(input) break;
    }

    // Find the item we're connecting from
    DuQFSlot *output = nullptr;
    foreach(QGraphicsItem *item, items(from))
    {
        output = qgraphicsitem_cast<DuQFSlot*>(item);
        if(output) break;
    }

    // Connect
    m_connectionManager.addConnection(output, input, m_connectingItem);
}

void DuQFNodeScene::initiateConnection(QPointF from)
{
    m_connecting = true;

    DuQFConnector *c = new DuQFConnector(from);
    this->addItem(c);
    m_connectingItem = c;
}

bool DuQFNodeScene::containsAll() const
{
    const double testMarginX = sceneRect().width() * m_margin;
    const double testMarginY = sceneRect().height() * m_margin;
    const QRectF testRect = sceneRect().adjusted(testMarginX, testMarginY, -testMarginX, -testMarginY);
    /*for (auto && item : items()) {
        if (dynamic_cast<Node *>(item)) {
            if (!testRect.contains(item->sceneBoundingRect())) {
                return false;
            }
        }
    }*/
    return true;
}

QRectF DuQFNodeScene::calculateRectangle(bool isForExport) const
{
    double nodeArea = 0;
    QRectF rect;
    int nodes = 0;
    /*for (auto && item : scene.items()) {
        if (auto node = dynamic_cast<Node *>(item)) {
            const auto nodeRect = node->placementBoundingRect();
            rect = rect.united(nodeRect.translated(node->pos().x(), node->pos().y()));
            nodeArea += nodeRect.width() * nodeRect.height();
            nodes++;
        }
    }*/

    const int margin = 60;

    if (isForExport) {
        return rect.adjusted(-margin, -margin, margin, margin);
    }

    // This "don't ask" heuristics tries to calculate a "nice" zoom-to-fit based on the design
    // density and node count. For example, if we have just a single node we don't want it to
    // be super big and cover the whole screen.
    const double density = nodeArea / rect.width() / rect.height();
    const double adjust = 3.0 * std::max(density * rect.width(), density * rect.height()) / pow(nodes, 1.5);
    return rect.adjusted(-adjust / 2, -adjust / 2, adjust / 2, adjust / 2).adjusted(-margin, -margin, margin, margin);
}

