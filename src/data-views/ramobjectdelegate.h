#ifndef RAMOBJECTDELEGATE_H
#define RAMOBJECTDELEGATE_H

#include <QStyledItemDelegate>
#include <QPainter>
#include <QPainterPath>
#include <QStringBuilder>

#include "duqf-app/app-style.h"
#include "ramses.h"

/**
 * @brief The RamObjectDelegate class is the main delegate used to paint RamObject in almost all the lists in Ramses.
 */
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

    void setComboBoxMode(bool comboBoxMode);

public slots:
    void setTimeTracking(bool newTimeTracking);
    void setCompletionRatio(bool newCompletionRatio);

signals:
    void editObject(RamObject*);
    void historyObject(RamObject*);
    void folderObject(RamObject*);

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
    int m_padding;

    // Buttons
    QPixmap m_editIcon;
    QPixmap m_historyIcon;
    QPixmap m_folderIcon;

    // Settings
    bool m_editable = false;
    bool m_comboBox = false;
    RamUser::UserRole m_editRole = RamUser::Admin;
    bool m_timeTracking = true;
    bool m_completionRatio = true;

    // Events
    bool m_editButtonPressed = false;
    bool m_editButtonHover = false;
    bool m_historyButtonPressed = false;
    bool m_historyButtonHover = false;
    bool m_folderButtonHover = false;
    bool m_folderButtonPressed = false;

    // Utils
    bool canEdit(const QModelIndex &index) const;

    // drawing specific items
    void drawMore(QPainter *painter, QRect rect, QPen pen) const;
    void drawButton(QPainter *painter, QRect rect, QPixmap icon, bool hover = false) const;
};

#endif // RAMOBJECTDELEGATE_H
