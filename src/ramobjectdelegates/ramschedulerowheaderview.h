#ifndef RAMSCHEDULEROWHEADERVIEW_H
#define RAMSCHEDULEROWHEADERVIEW_H

#include "ramschedulerow.h"
#include <QHeaderView>

class RamScheduleRowHeaderView : public QHeaderView
{
    Q_OBJECT
public:
    RamScheduleRowHeaderView(QWidget *parent = nullptr);

protected:
    virtual void paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const override;
    virtual QSize sectionSizeFromContents(int logicalIndex) const override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;

private:
    // Style
    QBrush m_abyss;
    QBrush m_dark;

    // Buttons
    QPixmap m_editIcon;

    // Events
    int m_editButtonPressed = -1;
    int m_editButtonHover = -1;

    // Utils
    bool canEdit() const;
    RamScheduleRow *getRow(int section) const;
    QRect editButtonRect(int section) const;
    RamScheduleRow *getScheduleRow(int section) const;
    // drawing specific items
    void drawButton(QPainter *painter, const QRect &rect, const QPixmap &icon, bool hover = false, bool checked = false) const;
};

#endif // RAMSCHEDULEROWHEADERVIEW_H
