#ifndef STATISTICSWIDGET_H
#define STATISTICSWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QGridLayout>

#include "ramobjectcombobox.h"
#include "statisticsview.h"
#include "smallwidgets/progresswidget.h"

class StatisticsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit StatisticsWidget(QWidget *parent = nullptr);

signals:

private slots:
    void projectChanged(RamProject*project);
    void updateEstimation(RamProject *project);
    void changeUser(RamObject *userObj);

private:
    void setupUi();
    void connectEvents();

    RamObjectComboBox *ui_userBox;
    ProgressWidget *ui_progressWidget;
    QLabel *ui_completionLabel;
    QLabel *ui_scheduledWorkLabel;
    QLabel *ui_remainingWorkLabel;
    QLabel *ui_latenessLabel;
    QLabel *ui_missingLabel;
    QLabel *ui_missingDaysLabel;
    StatisticsView *ui_statsTable;

    RamProject *m_project = nullptr;

};

#endif // STATISTICSWIDGET_H
