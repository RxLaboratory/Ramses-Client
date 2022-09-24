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
    DuQFNodeScene(DuQFGrid &grid, QObject *parent = nullptr);
    void adjustSceneRect();
    QRectF zoomToFit(bool isForExport = false) const;
    QVector<DuQFNode*> nodes();
    QVector<DuQFNode *> selectedNodes();
    QGraphicsItemGroup *createNodeGroup(QVector<DuQFNode *> nodes);

    DuQFConnectionManager *connectionManager() const;

public slots:
    void addNode();
    void addNode(DuQFNode *node, bool select = true);
    void removeSelectedConnections();
    void removeSelectedNodes();
    void removeSelection();
    void autoLayoutAll();
    void autoLayoutSelectedNodes();
    void autoLayoutNodes(QVector<DuQFNode*> nodes);
    void moveAllToCenter();
    void selectAllNodes();
    void selectNodes(QVector<DuQFNode *> nodes);
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
    void layoutNodesInColumn(QVector<DuQFNode*> nodes, QPointF center);
    void layoutNodesInColumn(QVector<DuQFNode *> nodes, qreal x, qreal y);
    void layoutGroupsInColumn(QVector<QGraphicsItemGroup *> groups);

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
