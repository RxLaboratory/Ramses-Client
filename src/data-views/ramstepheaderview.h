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

    // Settings
    bool m_timeTracking = true;
    bool m_completionRatio = true;
};

#endif // RAMSTEPHEADERVIEW_H
