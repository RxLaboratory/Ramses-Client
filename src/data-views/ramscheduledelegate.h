#ifndef RAMSCHEDULEDELEGATE_H
#define RAMSCHEDULEDELEGATE_H

#include <QStyledItemDelegate>
#include <QPainter>
#include <QPainterPath>

#include "duqf-app/app-style.h"
#include "ramses.h"
#include "ramobjectlistcombobox.h"
#include "ramschedulecomment.h"

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

    void showDetails(bool s);
    bool details() const;

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

    // Step icons
    QPixmap m_shotProdIcon;
    QPixmap m_assetProdIcon;
    QPixmap m_preProdIcon;
    QPixmap m_postProdIcon;

    // Events
    QModelIndex m_indexPressed;

    bool m_details = false;

    // drawing specific items
    void drawMore(QPainter *painter, QRect rect, QPen pen) const;
};

#endif // RAMSCHEDULEDELEGATE_H
