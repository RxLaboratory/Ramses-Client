#include "statisticswidget.h"

#include "ramses.h"

StatisticsWidget::StatisticsWidget(QWidget *parent) : QWidget(parent)
{
    setupUi();
    connectEvents();
    setProject();
}

void StatisticsWidget::setProject()
{
    m_project = Ramses::i()->project();
    Q_ASSERT(m_project);

    ui_userBox->setObjectModel(m_project->users(), "Users");
    updateEstimation(m_project);
    connect(m_project,SIGNAL(estimationComputed(RamProject*)),this,SLOT(updateEstimation(RamProject*)));

    ui_statsTable->resizeRowsToContents();
}

void StatisticsWidget::updateEstimation(RamProject *project)
{  
    //ui_progressWidget->setLatenessRatio( project->latenessRatio() );
    //ui_progressWidget->setTimeSpent( project->timeSpent() );
    //ui_progressWidget->setEstimation( project->estimation() );

    RamUser *user = RamUser::c( ui_userBox->currentObject() );
    QVector<float> stats = project->stats(user);

    float estimation = stats.at(0);
    float daysSpent = stats.at(1);
    float future = stats.at(3);
    float completion = 100;
    float missing = estimation - daysSpent - future;
    if (estimation > 0)
        completion = daysSpent / estimation * 100.0;

    ui_progressWidget->setCompletionRatio( completion );

    ui_scheduledWorkLabel->setText( QString::number(future, 'f', 0) + " days");

    ui_completionLabel->setText( QString::number( completion, 'f', 0 ) + " %" );

    ui_remainingWorkLabel->setText( QString::number( daysSpent, 'f', 0) + " / " + QString::number( estimation, 'f', 0 ) + " days" );

    if (missing > 1) {
        ui_missingDaysLabel->show();
        ui_missingLabel->setText("Missing: ");
        ui_missingDaysLabel->setText( QString::number( missing, 'f', 0) + " days" );
    }
    else if (missing < -1) {
        ui_missingDaysLabel->show();
        ui_missingLabel->setText("There are ");
        ui_missingDaysLabel->setText( QString::number( -missing, 'f', 0) + " extra days" );
    }
    else {
        ui_missingLabel->setText("The schedule seems to be OK");
        ui_missingDaysLabel->hide();
    }

    //ui_latenessLabel->setText( QString::number( project->latenessRatio(), 'f', 0) + " %");
}

void StatisticsWidget::changeUser(RamObject *userObj)
{
    RamUser *user = RamUser::c(userObj);
    ui_statsTable->setUser(user);
    ui_statsTable->resizeRowsToContents();
    updateEstimation(m_project);
}

void StatisticsWidget::setupUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(3);

    ui_userBox = new RamObjectComboBox(this);
    mainLayout->addWidget(ui_userBox);

    QVBoxLayout *projectLayout = new QVBoxLayout();
    projectLayout->setContentsMargins(0,0,0,0);
    projectLayout->setSpacing(3);

    QGridLayout *detailsLayout = new QGridLayout();
    detailsLayout->setContentsMargins(0,0,0,0);
    detailsLayout->setSpacing(3);

    QLabel *completionLabel = new QLabel("Completion: ", this);
    detailsLayout->addWidget(completionLabel, 0,1);

    ui_completionLabel = new QLabel("-- %", this);
    detailsLayout->addWidget(ui_completionLabel, 0, 2);

    ui_remainingWorkLabel = new QLabel("-- days (done) / -- days (total)");
    detailsLayout->addWidget(ui_remainingWorkLabel, 1, 2);

    QLabel *scheduled = new QLabel("Scheduled: ", this);
    detailsLayout->addWidget(scheduled, 2, 1);

    ui_scheduledWorkLabel = new QLabel("-- days", this);
    detailsLayout->addWidget(ui_scheduledWorkLabel, 2, 2);

    ui_missingLabel = new QLabel("Missing: ", this);
    detailsLayout->addWidget(ui_missingLabel, 3, 1);

    ui_missingDaysLabel = new QLabel("-- days", this);
    detailsLayout->addWidget(ui_missingDaysLabel, 3, 2);

    //QLabel *latenessLabel = new QLabel("Estimated lateness: ", this);
    //detailsLayout->addWidget(latenessLabel, 4,1);

    //ui_latenessLabel = new QLabel("-- %", this);
    //detailsLayout->addWidget(ui_latenessLabel, 4, 2);

    detailsLayout->setColumnStretch(0, 100);
    detailsLayout->setColumnStretch(1, 0);
    detailsLayout->setColumnStretch(2, 0);
    detailsLayout->setColumnStretch(3, 100);

    projectLayout->addLayout(detailsLayout);

    ui_progressWidget = new ProgressWidget(this);
    projectLayout->addWidget(ui_progressWidget);

    mainLayout->addLayout(projectLayout);

    // Add the stats table
    ui_statsTable = new StatisticsView(this);

    mainLayout->addWidget(ui_statsTable);
}

void StatisticsWidget::connectEvents()
{
    connect(ui_userBox, SIGNAL(currentObjectChanged(RamObject*)), this, SLOT(changeUser(RamObject*)));
}
