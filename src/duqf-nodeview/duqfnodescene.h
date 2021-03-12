#ifndef DUQFNODESCENE_H
#define DUQFNODESCENE_H

#include <QGraphicsScene>
#include <cmath>

#include <QGraphicsRectItem>

class DuQFNodeScene : public QGraphicsScene
{
    Q_OBJECT
public:
    DuQFNodeScene();
    void adjustSceneRect();
    QRectF zoomToFit(bool isForExport = false) const;

private:
    bool containsAll() const;
    QRectF calculateRectangle(bool isForExport = false) const;

    int m_initialSize = 10000;
    double m_margin = .25;
};

#endif // DUQFNODESCENE_H
