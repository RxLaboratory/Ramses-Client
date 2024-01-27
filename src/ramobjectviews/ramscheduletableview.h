#ifndef RAMSCHEDULETABLEVIEW_H
#define RAMSCHEDULETABLEVIEW_H

#include <QHeaderView>

#include "duqf-widgets/dutableview.h"
#include "ramscheduledelegate.h"

class RamScheduleTableView : public DuTableView
{
    Q_OBJECT
public:
    RamScheduleTableView(QWidget *parent = nullptr);

public slots:
    void showDetails(bool s);

private slots:

private:
    void setupUi();
    void connectEvents();

    RamScheduleDelegate *m_delegate;

    // UI Events
    QPoint m_initialDragPos;
    bool m_dragging = false;
};

#endif // RAMSCHEDULETABLEVIEW_H
