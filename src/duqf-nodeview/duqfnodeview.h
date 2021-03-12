#ifndef DUQFNODEVIEW_H
#define DUQFNODEVIEW_H

#include <QGraphicsView>
#include <cmath>
#include <QGraphicsItemGroup>

#include "duqf-app/app-style.h"
#include "duqf-nodeview/duqfgrid.h"

class DuQFNodeView : public QGraphicsView
{
    Q_OBJECT
public:
    enum Transformation { Zoom, Pan, None };
    Q_ENUM(Transformation)

    DuQFNodeView(QWidget *parent = nullptr);
    const DuQFGrid & grid() const;
    void zoom(double amount);
    qreal currentZoom();

public slots:
    void frameSelected();
    void reinitTransform();
    void setZoom(int zoomPercent);

signals:
    void zoomed(qreal zoom);
    void zoomed(int zoomPercent);

protected:
    void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
    void keyReleaseEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

private:
    void drawBackground(QPainter * painter, const QRectF & rect) Q_DECL_OVERRIDE;

    // members
    DuQFGrid m_grid;
    bool m_gridVisible = true;
    QRectF m_nodeBoundingRect;

    // settings
    double m_zoomSensitivity = 0.2;

    // interaction
    bool m_ctrlKeyPressed = false;
    bool m_mouseMiddlePressed = false;
    Transformation m_mouseTransformation = None;
    QPointF m_mousePosition;
};

#endif // DUQFNODEVIEW_H
