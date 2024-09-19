#ifndef TIMELINEWIDGET_H
#define TIMELINEWIDGET_H

#include <QWidget>

#include "duwidgets/dudoubleslider.h"
#include "timelineview.h"

class TimelineWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TimelineWidget(QWidget *parent = nullptr);

private:
    void setupUi();
    void connectEvents();

    TimelineView *ui_timeline;
    DuDoubleSlider *ui_zoomSlider;
};

#endif // TIMELINEWIDGET_H
