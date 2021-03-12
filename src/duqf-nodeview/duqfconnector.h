#ifndef DUQFCONNECTOR_H
#define DUQFCONNECTOR_H

#include <QGraphicsObject>
#include <QPainter>
#include <QGraphicsSceneHoverEvent>

#include "duqf-app/app-style.h"

class DuQFConnector : public QGraphicsObject
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
public:
    enum Type{ All, Input, Output };
    Q_ENUM(Type)

    DuQFConnector(Type type = All, QColor color = QColor());

    QRectF boundingRect() const Q_DECL_OVERRIDE;
    void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = nullptr) Q_DECL_OVERRIDE;

    Type connectorType() const;

signals:
    void connectionInitiated(QPointF);
    void connectionMoved(QPointF);
    void connectionFinished(QPointF, QPointF);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) Q_DECL_OVERRIDE;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) Q_DECL_OVERRIDE;

private:
    // Attributes
    Type m_type;

    // Appearance
    QSizeF m_size;
    QColor m_color;
    QRectF m_boundingRect;

    // Interactions
    bool m_hover = false;
    bool m_connecting = false;
};

#endif // DUQFCONNECTOR_H
