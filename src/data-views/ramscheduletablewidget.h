#ifndef RAMSCHEDULETABLEWIDGET_H
#define RAMSCHEDULETABLEWIDGET_H

#include <QTableView>
#include <QHeaderView>
#include <QScrollBar>

#include "ramscheduledelegate.h"

class RamScheduleTableWidget : public QTableView
{
    Q_OBJECT
public:
    RamScheduleTableWidget(QWidget *parent = nullptr);

public slots:
    void showDetails(bool s);

protected:
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;

private slots:

private:
    void setupUi();
    void connectEvents();

    RamScheduleDelegate *m_delegate;

    // UI Events
    QPoint m_initialDragPos;
    bool m_dragging = false;
};

#endif // RAMSCHEDULETABLEWIDGET_H
