#ifndef RAMOBJECTDELEGATE_H
#define RAMOBJECTDELEGATE_H

#include <QStyledItemDelegate>
#include <QPainter>
#include <QPainterPath>
#include <QStringBuilder>

#include "duqf-app/app-style.h"
#include "ramobject.h"
#include "ramuser.h"
#include "ramproject.h"

class RamObjectDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    RamObjectDelegate(QObject *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const Q_DECL_OVERRIDE;
    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const Q_DECL_OVERRIDE;

private:
    // Style
    QColor m_abyss;
    QColor m_dark;
    QColor m_medium;
    QColor m_lessLight;
    QColor m_light;
    QFont m_textFont;
    QFont m_detailsFont;
    QTextOption m_textOption;
    QTextOption m_detailsOption;
    int m_padding;

    // drawing specific items
    void drawMore(QPainter *painter, QRect rect, QPen pen) const;
};

#endif // RAMOBJECTDELEGATE_H
