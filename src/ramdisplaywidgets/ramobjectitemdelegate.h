#ifndef RAMOBJECTITEMDELEGATE_H
#define RAMOBJECTITEMDELEGATE_H

#include <QStyledItemDelegate>
#include <QPainter>
#include <QPainterPath>

#include "duqf-app/app-style.h"
#include "ramobject.h"

class RamObjectItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    RamObjectItemDelegate(QObject *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const Q_DECL_OVERRIDE;
    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const Q_DECL_OVERRIDE;

private:
    // Style
    QPen m_textPen;
    QPen m_hoverTextPen;
    QBrush m_bgBrush;
    QBrush m_selectedBgBrush;
    QBrush m_hoverBgBrush;
    QFont m_textFont;
    QTextOption m_textOption;
    int m_padding;
};

#endif // RAMOBJECTITEMDELEGATE_H
