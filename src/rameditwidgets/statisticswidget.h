#ifndef STATISTICSWIDGET_H
#define STATISTICSWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QGridLayout>

#include "ramstatisticstablewidget.h"
#include "data-models/ramstatisticstable.h"
#include "smallwidgets/progresswidget.h"

class StatisticsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit StatisticsWidget(QWidget *parent = nullptr);

signals:

private slots:
    void projectChanged(RamProject*project);
    void estimationChanged(RamProject *project);

private:
    void setupUi();
    void connectEvents();

    ProgressWidget *ui_progressWidget;
    QLabel *ui_completionLabel;
    QLabel *ui_remainingTimeLabel;
    QLabel *ui_latenessLabel;
    QLabel *ui_estimationLabel;
    QLabel *ui_estimationUserLabel;
    QLabel *ui_unassignedTitleLabel;
    QLabel *ui_unassignedLabel;

    RamProject *m_project = nullptr;

};

#endif // STATISTICSWIDGET_H
