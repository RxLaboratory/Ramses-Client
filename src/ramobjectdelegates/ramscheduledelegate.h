#ifndef RAMSCHEDULEDELEGATE_H
#define RAMSCHEDULEDELEGATE_H

#include <QStyledItemDelegate>
#include <QPainter>
#include <QPainterPath>

#include "ramobject.h"
#include "ramschedulecomment.h"
#include "ramuser.h"

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

protected:
    int drawMarkdown(QPainter *painter, QRect rect, const QString &md) const;

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

    bool m_details = false;

    // drawing specific items
    void drawMore(QPainter *painter, QRect rect, QPen pen) const;

    QWidget *m_currentEditor = nullptr;

    // Utils
    RamUser *getUser(QModelIndex index) const;
    QDateTime getDate(QModelIndex index) const;
    RamScheduleEntry *getEntry(QModelIndex index) const;
    RamScheduleComment *getComment(QModelIndex index) const;
    QSet<RamStatus*> getDueTasks(QModelIndex index) const;

};

#endif // RAMSCHEDULEDELEGATE_H
