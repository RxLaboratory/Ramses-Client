#ifndef DUQFNODESCENE_H
#define DUQFNODESCENE_H

#include <QGraphicsScene>
#include <cmath>

#include <QGraphicsRectItem>

#include "duqf-nodeview/duqfnode.h"
#include "duqf-nodeview/duqfconnection.h"
#include "duqf-nodeview/duqfconnectionmanager.h"

class DuQFNodeScene : public QGraphicsScene
{
    Q_OBJECT
public:
    DuQFNodeScene();
    void adjustSceneRect();
    QRectF zoomToFit(bool isForExport = false) const;

public slots:
    void addNode();

private slots:
    void initiateConnection(QPointF from);
    void moveConnection(QPointF to);
    void finishConnection(QPointF to, QPointF from);

private:
    bool containsAll() const;
    QRectF calculateRectangle(bool isForExport = false) const;

    int m_initialSize = 10000;
    double m_margin = .25;

    // Interactions
    bool m_connecting = false;
    DuQFConnection *m_connectingItem = nullptr;

    QList<DuQFConnectionManager*> m_connections;
};

#endif // DUQFNODESCENE_H
