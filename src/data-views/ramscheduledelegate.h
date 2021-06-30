#ifndef RAMSCHEDULEDELEGATE_H
#define RAMSCHEDULEDELEGATE_H

#include <QStyledItemDelegate>
#include <QPainter>
#include <QPainterPath>

#include "duqf-app/app-style.h"
#include "ramses.h"
#include "ramobjectlistcombobox.h"

class RamScheduleDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit RamScheduleDelegate(QObject *parent = nullptr);
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const override;

    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override;

private slots:
    void setEntry(RamObject *stepObj);

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

    // Events
    QModelIndex m_indexPressed;

    // drawing specific items
    void drawMore(QPainter *painter, QRect rect, QPen pen) const;
};

#endif // RAMSCHEDULEDELEGATE_H
