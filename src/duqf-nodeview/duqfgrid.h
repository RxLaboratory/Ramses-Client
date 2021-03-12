#ifndef DUQFGRID_H
#define DUQFGRID_H

#include <QPoint>

class DuQFGrid
{
public:
    DuQFGrid();
    void setSize(int gridSize);
    int size() const;
    QPointF snapToGrid(QPointF in) const;
private:
    int _size = 20;
};

#endif // DUQFGRID_H
