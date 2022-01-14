#ifndef RAMSTATISTICSDELEGATE_H
#define RAMSTATISTICSDELEGATE_H

#include <QStyledItemDelegate>
#include <QPainter>
#include <QPainterPath>

#include "duqf-app/app-style.h"

class RamStatisticsDelegate : public QStyledItemDelegate
{
public:
    explicit RamStatisticsDelegate(QObject *parent = nullptr);
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const override;

private:
    // Style
    QColor m_abyss;
    QColor m_dark;
    QColor m_medium;
    QColor m_lessLight;
    QColor m_light;
    QFont m_textFont;
    QFont m_detailsFont;
    int m_padding;

    // drawing specific items
    void drawMore(QPainter *painter, QRect rect, QPen pen) const;

    // Settings
    bool m_timeTracking = true;
    bool m_completionRatio = true;
};

#endif // RAMSTATISTICSDELEGATE_H
