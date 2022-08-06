#ifndef RAMSTATISTICSTABLEWIDGET_H
#define RAMSTATISTICSTABLEWIDGET_H

#include <QTableView>
#include <QHeaderView>
#include <QScrollBar>
#include <QMouseEvent>

#include "data-models/ramstatisticstable.h"

class RamStatisticsTableWidget : public QTableView
{
public:
    RamStatisticsTableWidget(QWidget *parent = nullptr);
    void setUser(RamUser *user);

protected:
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void showEvent(QShowEvent *event) override;
    virtual void resizeEvent(QResizeEvent *event) override;

private:
    void setupUi();
    void connectEvents();

    // Models
    RamStatisticsTable *m_statsModel;

    // UI Events
    QPoint m_initialDragPos;
    bool m_dragging = false;
};

#endif // RAMSTATISTICSTABLEWIDGET_H
