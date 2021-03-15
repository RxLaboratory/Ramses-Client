#ifndef DUQFNODE_H
#define DUQFNODE_H

#include <QGraphicsObject>
#include <QGraphicsDropShadowEffect>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsTextItem>
#include <QApplication>

#include "duqf-app/app-style.h"

#include "duqf-nodeview/duqfslot.h"

class DuQFNode : public QGraphicsObject
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
public:
    DuQFNode(QString title = "Node");
    //! Copy constructor.
    DuQFNode(const DuQFNode & other);
    ~DuQFNode() Q_DECL_OVERRIDE;

    QRectF boundingRect() const Q_DECL_OVERRIDE;
    void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = nullptr) Q_DECL_OVERRIDE;

    QString title() const;
    void setTitle(const QString &title);

signals:
    void connectionInitiated(QPointF);
    void connectionMoved(QPointF);
    void connectionFinished(QPointF, QPointF);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent * event) Q_DECL_OVERRIDE;

private:
    // Appearance
    //QSizeF m_size = QSizeF(150.0,30.0);
    int m_cornerRadius = 5;
    int m_padding = 5;
    QRectF m_boundingRect;

    // Children
    QGraphicsTextItem *m_titleItem;
    DuQFSlot *m_defaultInputConnector;
    DuQFSlot *m_defaultOutputConnector;
};

#endif // DUQFNODE_H
