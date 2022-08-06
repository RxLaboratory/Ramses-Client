#ifndef RAMOBJECTDELEGATE_H
#define RAMOBJECTDELEGATE_H

#include <QStyledItemDelegate>
#include <QPainter>
#include <QPainterPath>
#include <QStringBuilder>

#include "ramuser.h"

struct PaintParameters
{
    QColor bgColor;
    QColor textColor;
    QColor detailsColor;
    QRect bgRect;
    QRect iconRect;
    QRect detailsRect;
    QRect titleRect;
};

/**
 * @brief The RamObjectDelegate class is the main delegate used to paint RamObject in almost all the lists in Ramses.
 */
template<typename RO> class RamObjectDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    RamObjectDelegate(QObject *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;
    virtual QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const override;

    void setEditable(bool editable);
    void setEditRole(RamUser::UserRole role);

    void setComboBoxMode(bool comboBoxMode);

public slots:
    void setTimeTracking(bool newTimeTracking);
    void setCompletionRatio(bool newCompletionRatio);
    void showDetails(bool s);

signals:
    void editObject(RO);
    void historyObject(RO);
    void folderObject(RO);

protected:
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override;

    // Utils
    RO getObject(const QModelIndex &index) const;

    // drawing specific items
    void drawMore(QPainter *painter, QRect rect, QPen pen) const;
    void drawButton(QPainter *painter, QRect rect, QPixmap icon, bool hover = false) const;

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
    QPixmap m_historyDarkIcon;
    QPixmap m_folderDarkIcon;

    // Buttons hover
    QModelIndex m_historyButtonHover = QModelIndex();
    QModelIndex m_folderButtonHover = QModelIndex();

private:
    // UTILS //
    PaintParameters getPaintParameters(const QStyleOptionViewItem &option, RO obj = nullptr) const;
    void paintBG(QPainter *painter, PaintParameters *params) const;
    void paintTitle(RO obj, QPainter *painter, PaintParameters *params) const;
    void paintTitle(QString title, QPainter *painter, PaintParameters *params) const;
    void paintButtons(RO obj, QPainter *painter, PaintParameters *params, const QModelIndex &index) const;
    void paintDetails(QString details, QPainter *painter, PaintParameters *params) const;
    void paintDetails(RO obj, QPainter *painter, PaintParameters *params) const;


    // Settings
    bool m_editable = false;
    bool m_comboBox = false;
    RamUser::UserRole m_editRole = RamUser::Admin;
    bool m_timeTracking = true;
    bool m_completionRatio = true;
    bool m_details = false;

    // Events
    bool m_historyButtonPressed = false;
    bool m_cellPressed = false;
    bool m_folderButtonPressed = false;
};

#endif // RAMOBJECTDELEGATE_H
