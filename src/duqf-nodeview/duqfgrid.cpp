#include "duqfgrid.h"

DuQFGrid::DuQFGrid()
{

}

void DuQFGrid::setSize(int size)
{
    _size = size;
}

int DuQFGrid::size() const
{
    return _size;
}

QPointF DuQFGrid::snapToGrid(QPointF in) const
{
    if (!_size) {
        return in;
    }

    return {
        static_cast<double>(static_cast<int>(in.x() / _size) * _size),
        static_cast<double>(static_cast<int>(in.y() / _size) * _size)
    };
}
