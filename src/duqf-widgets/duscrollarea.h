#ifndef DUSCROLLAREA_H
#define DUSCROLLAREA_H

#include <QScrollArea>

class DuScrollArea : public QScrollArea
{
    Q_OBJECT
public:
    DuScrollArea(QWidget *parent = nullptr);

protected:
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;

private:
    // UI Events
    QPoint _initialDragPos;
    bool _dragging = false;
};

#endif // DUSCROLLAREA_H
