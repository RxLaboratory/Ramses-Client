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
    DuQFNodeScene(DuQFGrid &grid);
    void adjustSceneRect();
    QRectF zoomToFit(bool isForExport = false) const;

public slots:
    void addNode();
    void removeSelectedConnections();
    void removeSelectedNodes();

private slots:
    void initiateConnection(QPointF from);
    void moveConnection(QPointF to);
    void finishConnection(QPointF to, QPointF from);

private:
    DuQFGrid &m_grid;

    bool containsAll() const;
    QRectF calculateRectangle(bool isForExport = false) const;

    int m_initialSize = 10000;
    double m_margin = .25;

    // Interactions
    bool m_connecting = false;
    DuQFConnector *m_connectingItem = nullptr;

    DuQFConnectionManager m_connectionManager;
};

#endif // DUQFNODESCENE_H
