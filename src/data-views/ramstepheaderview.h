#ifndef RAMSTEPHEADERVIEW_H
#define RAMSTEPHEADERVIEW_H

#include <QHeaderView>
#include <QtDebug>
#include <QPainter>
#include <QPainterPath>

#include "ramstep.h"
#include "ramstatus.h"

class RamStepHeaderView : public QHeaderView
{
    Q_OBJECT
public:
    RamStepHeaderView(QWidget *parent = nullptr);

public slots:
    void setTimeTracking(bool newTimeTracking);
    void setCompletionRatio(bool newCompletionRatio);

protected:
    virtual void paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;

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
    QPixmap m_folderIcon;

    // Settings
    bool m_timeTracking = true;
    bool m_completionRatio = true;

    // Events
    int m_editButtonPressed = -1;
    int m_editButtonHover = -1;
    int m_folderButtonHover = -1;
    int m_folderButtonPressed = -1;

    // Utils
    bool canEdit() const;

    // drawing specific items
    void drawButton(QPainter *painter, QRect rect, QPixmap icon, bool hover = false) const;
};

#endif // RAMSTEPHEADERVIEW_H
