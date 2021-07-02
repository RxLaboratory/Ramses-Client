#include "statisticswidget.h"

StatisticsWidget::StatisticsWidget(QWidget *parent) : QWidget(parent)
{
    setupUi();
    connectEvents();
}

void StatisticsWidget::projectChanged(RamProject *project)
{
    if (m_project) disconnect(m_project, nullptr, this, nullptr);

    m_project = project;

    if (!project) return;

    estimationChanged(project);
    connect(m_project,SIGNAL(estimationComputed(RamProject*)),this,SLOT(estimationChanged(RamProject*)));
}

void StatisticsWidget::estimationChanged(RamProject *project)
{
    ui_progressWidget->setCompletionRatio( project->completionRatio() );
    ui_progressWidget->setLatenessRatio( project->latenessRatio() );
    ui_progressWidget->setTimeSpent( project->timeSpent() );
    ui_progressWidget->setEstimation( project->estimation() );

    int remainingDays = QDate::currentDate().daysTo( project->deadline() );
    ui_remainingTimeLabel->setText( QString::number(remainingDays) + " days");

    ui_completionLabel->setText( QString::number( project->completionRatio() ) + " %");

    ui_latenessLabel->setText( QString::number( project->latenessRatio(), 'f', 0) + " %");

    ui_estimationLabel->setText( QString::number( project->estimation(), 'f', 0) + " days");

    if (project->users()->count() > 0)
        ui_estimationUserLabel->setText( QString::number( project->estimation() / project->users()->count(), 'f', 0) + " days/user" );
    else
        ui_estimationUserLabel->setText("");

    if (project->unassignedDays() > 0)
    {
        ui_unassignedLabel->show();
        ui_unassignedTitleLabel->show();
        ui_unassignedLabel->setText( QString::number( project->unassignedDays(), 'f', 0) + " days");
    }
    else
    {
        ui_unassignedLabel->hide();
        ui_unassignedTitleLabel->hide();
    }
}

void StatisticsWidget::setupUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(3);

    QVBoxLayout *projectLayout = new QVBoxLayout();
    projectLayout->setContentsMargins(0,0,0,0);
    projectLayout->setSpacing(3);

    QGridLayout *detailsLayout = new QGridLayout();
    detailsLayout->setContentsMargins(0,0,0,0);
    detailsLayout->setSpacing(3);

    QLabel *timeRemaining = new QLabel("Time remaining: ", this);
    detailsLayout->addWidget(timeRemaining, 0, 1);

    ui_remainingTimeLabel = new QLabel("-- days", this);
    detailsLayout->addWidget(ui_remainingTimeLabel, 0, 2);

    QLabel *completionLabel = new QLabel("Completion: ", this);
    detailsLayout->addWidget(completionLabel, 1,1);

    ui_completionLabel = new QLabel("-- %", this);
    detailsLayout->addWidget(ui_completionLabel, 1, 2);

    QLabel *latenessLabel = new QLabel("Lateness: ", this);
    detailsLayout->addWidget(latenessLabel, 2,1);

    ui_latenessLabel = new QLabel("-- %", this);
    detailsLayout->addWidget(ui_latenessLabel, 2, 2);

    QLabel *estimationLabel = new QLabel("Estimation: ", this);
    detailsLayout->addWidget(estimationLabel, 3,1);

    ui_estimationLabel = new QLabel("-- days", this);
    detailsLayout->addWidget(ui_estimationLabel, 3, 2);

    ui_estimationUserLabel = new QLabel("-- days/user", this);
    detailsLayout->addWidget(ui_estimationUserLabel, 4, 2);

    ui_unassignedTitleLabel = new QLabel("Missing: ", this);
    detailsLayout->addWidget(ui_unassignedTitleLabel, 5,1);

    ui_unassignedLabel = new QLabel("-- days", this);
    detailsLayout->addWidget(ui_unassignedLabel, 5,2);


    detailsLayout->setColumnStretch(0, 100);
    detailsLayout->setColumnStretch(1, 0);
    detailsLayout->setColumnStretch(2, 0);
    detailsLayout->setColumnStretch(3, 100);

    projectLayout->addLayout(detailsLayout);

    ui_progressWidget = new ProgressWidget(this);
    projectLayout->addWidget(ui_progressWidget);

    mainLayout->addLayout(projectLayout);

    // Add the stats table
    RamStatisticsTableWidget *statsTable = new RamStatisticsTableWidget(this);
    statsTable->setModel(new RamStatisticsTable(this));

    mainLayout->addWidget(statsTable);
}

void StatisticsWidget::connectEvents()
{
    connect(Ramses::instance(), SIGNAL(currentProjectChanged(RamProject*)), this, SLOT(projectChanged(RamProject*)));
}
