#ifndef DUQFNODESCENE_H
#define DUQFNODESCENE_H

#include <QGraphicsScene>
#include <cmath>

#include <QGraphicsRectItem>

#include "duqfnode.h"
#include "duqfconnector.h"
#include "duqfconnection.h"
#include "duqfconnectionmanager.h"
#include "duqfgrid.h"

class DuQFNodeScene : public QGraphicsScene
{
    Q_OBJECT
public:
    enum LayoutAlgorithm {
        Layered,
        Legacy,
    };

    DuQFNodeScene(DuQFGrid &grid, QObject *parent = nullptr);
    void adjustSceneRect();
    QRectF zoomToFit(bool isForExport = false) const;
    QSet<DuQFNode *> nodes();
    QSet<DuQFNode *> selectedNodes();
    QGraphicsItemGroup *createNodeGroup(QSet<DuQFNode *> nodes);

    DuQFConnectionManager *connectionManager() const;

public slots:
    void addNode();
    void addNode(DuQFNode *node, bool select = true);
    void removeSelectedConnections();
    void removeSelectedNodes();
    void removeSelection();
    void autoLayoutAll();
    void autoLayoutSelectedNodes();
    void autoLayoutNodes(QSet<DuQFNode *> nodes, LayoutAlgorithm algo = Layered);
    void moveAllToCenter();
    void selectAllNodes();
    void selectNodes(QSet<DuQFNode *> nodes);
    void selectChildNodes();
    void selectParentNodes();
    DuQFConnection *connectNodes(DuQFNode *o, DuQFNode *i);

private slots:
    void initiateConnection(QPointF from);
    void moveConnection(QPointF to);
    void finishConnection(QPointF to, QPointF from);
    void nodeRemoved(DuQFNode *node);

private:
    DuQFGrid &m_grid;

    // Utils
    void layoutNodesInColumn(QSet<DuQFNode*> nodes, QPointF center);
    void layoutNodesInColumn(QSet<DuQFNode *> nodes, qreal x, qreal y);
    void layoutGroupsInColumn(QSet<QGraphicsItemGroup *> groups);

    // TEMP
    bool containsAll() const;
    QRectF calculateRectangle(bool isForExport = false) const;

    // Settings
    int m_initialSize = 10000;
    double m_margin = .25;

    // Interactions
    bool m_connecting = false;
    DuQFConnector *m_connectingItem = nullptr;

    DuQFConnectionManager *m_connectionManager;
};

#endif // DUQFNODESCENE_H
