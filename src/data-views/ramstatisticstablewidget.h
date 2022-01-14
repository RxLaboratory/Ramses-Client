#ifndef RAMSTATISTICSTABLEWIDGET_H
#define RAMSTATISTICSTABLEWIDGET_H

#include <QTableView>
#include <QHeaderView>
#include <QScrollBar>
#include <QMouseEvent>

#include "ramstatisticsdelegate.h"

class RamStatisticsTableWidget : public QTableView
{
public:
    RamStatisticsTableWidget(QWidget *parent = nullptr);

protected:
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void showEvent(QShowEvent *event) override;
    virtual void resizeEvent(QResizeEvent *event) override;

private:
    void setupUi();
    void connectEvents();

    // UI Events
    QPoint m_initialDragPos;
    bool m_dragging = false;
};

#endif // RAMSTATISTICSTABLEWIDGET_H
