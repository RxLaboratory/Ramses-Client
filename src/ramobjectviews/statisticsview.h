#ifndef STATISTICSIEW_H
#define STATISTICSIEW_H

#include <QHeaderView>
#include <QMouseEvent>

#include "duwidgets/dutableview.h"
#include "statisticsmodel.h"

class StatisticsView : public DuTableView
{
public:
    StatisticsView(QWidget *parent = nullptr);
    void setUser(RamUser *user);

protected:
    virtual void showEvent(QShowEvent *event) override;
    virtual void resizeEvent(QResizeEvent *event) override;

private:
    void setupUi();

    // Models
    StatisticsModel *m_statsModel;

    // UI Events
    QPoint m_initialDragPos;
    bool m_dragging = false;
};

#endif // STATISTICSIEW_H
