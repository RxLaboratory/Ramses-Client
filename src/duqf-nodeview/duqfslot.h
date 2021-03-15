#ifndef DUQFSLOT_H
#define DUQFSLOT_H

#include <QGraphicsObject>
#include <QPainter>
#include <QGraphicsSceneHoverEvent>

#include "duqf-app/app-style.h"

class DuQFSlot : public QGraphicsObject
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
public:
    enum SlotType{ All = 0,
               Input = 1,
               Output = 2 };
    Q_ENUM(SlotType)

    enum { Type = UserType + 2 };
    int type() const Q_DECL_OVERRIDE { return Type; }

    DuQFSlot(SlotType type = All, bool singleConnection = false, QColor color = QColor());

    QRectF boundingRect() const Q_DECL_OVERRIDE;
    void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = nullptr) Q_DECL_OVERRIDE;

    SlotType connectorType() const;

    bool isSingleConnection() const;
    void setSingleConnection(bool singleConnection);

    SlotType slotType() const;
    void setSlotType(const SlotType &slotType);

public slots:
    void remove();

signals:
    void connectionInitiated(QPointF);
    void connectionMoved(QPointF);
    void connectionFinished(QPointF, QPointF);
    void removed();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) Q_DECL_OVERRIDE;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) Q_DECL_OVERRIDE;

private:
    bool m_removing = false;

    // Attributes
    SlotType m_slotType;
    bool m_singleConnection;

    // Appearance
    QSizeF m_size;
    QColor m_color;
    QRectF m_boundingRect;

    // Interactions
    bool m_hover = false;
    bool m_connecting = false;
};

#endif // DUQFSLOT_H
