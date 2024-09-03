#ifndef DUQFCONNECTOR_H
#define DUQFCONNECTOR_H

#include <QGraphicsObject>
#include <QPainter>

struct CurveHandles {
    QPointF from;
    QPointF to;
};

class DuQFConnector : public QGraphicsObject
{
    Q_OBJECT
public:
    DuQFConnector(QString title = "", QGraphicsItem *parent = nullptr);
    DuQFConnector(QPointF from, QString title = "");

    enum { Type = UserType + 1 };
    int type() const override { return Type; }

    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = nullptr) override;

    QString title() const;
    void setTitle(const QString &title);

    void setFromColor(const QColor &newFromColor);

    void setToColor(const QColor &newToColor);

public slots:
    void setTo(const QPointF &to);
    void setFrom(const QPointF &from);
    void remove();

signals:
    void removed();
    void selected(bool);
    void clicked();

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

private:
    void setupUi();
    void updateTitlePos();

    bool m_removing = false;

    QPointF m_from;
    QPointF m_to;
    qreal m_width;

    QColor m_fromColor;
    QColor m_toColor;

    // Appearance
    int m_cornerRadius = 5;
    int m_padding = 5;

    // Children
    QGraphicsTextItem *m_titleItem;

    // Utils
    CurveHandles curveHandles(float q=1.0) const;
};

#endif // DUQFCONNECTOR_H
