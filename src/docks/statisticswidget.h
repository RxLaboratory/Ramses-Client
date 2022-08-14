#ifndef STATISTICSWIDGET_H
#define STATISTICSWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QGridLayout>

#include "data-views/ramobjectlistcombobox.h"
#include "ramstatisticstablewidget.h"
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
    void changeUser(RamUser *user);

private:
    void setupUi();
    void connectEvents();

    RamObjectListComboBox<RamUser*> *ui_userBox;
    ProgressWidget *ui_progressWidget;
    QLabel *ui_completionLabel;
    QLabel *ui_remainingTimeLabel;
    QLabel *ui_scheduledWorkLabel;
    QLabel *ui_remainingWorkLabel;
    QLabel *ui_latenessLabel;
    RamStatisticsTableWidget *ui_statsTable;

    RamProject *m_project = nullptr;

};

#endif // STATISTICSWIDGET_H
