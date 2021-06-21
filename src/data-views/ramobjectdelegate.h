#ifndef RAMOBJECTDELEGATE_H
#define RAMOBJECTDELEGATE_H

#include <QStyledItemDelegate>
#include <QPainter>
#include <QPainterPath>
#include <QStringBuilder>

#include "duqf-app/app-style.h"
#include "ramses.h"

class RamObjectDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    RamObjectDelegate(QObject *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const override;

    void setEditable(bool editable);
    void setEditRole(RamUser::UserRole role);

signals:
    void editObject(RamObject*);

protected:
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override;

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

    // Settings
    bool m_editable = false;
    bool m_editRole = RamUser::Admin;

    // Events
    bool m_editButtonPressed = false;
    bool m_editButtonHover = false;

    // Utils
    bool canEdit() const;

    // drawing specific items
    void drawMore(QPainter *painter, QRect rect, QPen pen) const;
};

#endif // RAMOBJECTDELEGATE_H
