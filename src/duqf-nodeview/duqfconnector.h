#ifndef DUQFCONNECTOR_H
#define DUQFCONNECTOR_H

#include <QGraphicsObject>
#include <QPainter>
#include <QSettings>

#include "duqf-app/app-style.h"

class DuQFConnector : public QGraphicsObject
{
    Q_OBJECT
public:
    DuQFConnector(QPointF from);

    enum { Type = UserType + 1 };
    int type() const Q_DECL_OVERRIDE { return Type; }

    QRectF boundingRect() const Q_DECL_OVERRIDE;
    QPainterPath shape() const Q_DECL_OVERRIDE;
    void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = nullptr) Q_DECL_OVERRIDE;

public slots:
    void setTo(const QPointF &to);
    void setFrom(const QPointF &from);
    void remove();

signals:
    void removed();

private:
    bool m_removing = false;

    QPointF m_from;
    QPointF m_to;
    qreal m_width;

    // Coordinates of the handles
    QPointF m_fromHandle;
    QPointF m_toHandle;
};

#endif // DUQFCONNECTOR_H
