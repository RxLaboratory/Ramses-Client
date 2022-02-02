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
    int completion = project->completionRatio();
    int estimation = project->estimation();

    ui_remainingTimeLabel->setText( QString::number(remainingDays) + " days");

    ui_completionLabel->setText( QString::number( completion ) + " %" );

    ui_remainingWorkLabel->setText( QString::number( int(completion * estimation / 100) ) + " / " + QString::number( estimation ) + " days" );

    ui_latenessLabel->setText( QString::number( project->latenessRatio(), 'f', 0) + " %");
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

    QLabel *timeRemaining = new QLabel("Remaining time: ", this);
    detailsLayout->addWidget(timeRemaining, 0, 1);

    ui_remainingTimeLabel = new QLabel("-- days", this);
    detailsLayout->addWidget(ui_remainingTimeLabel, 0, 2);

    QLabel *completionLabel = new QLabel("Completion: ", this);
    detailsLayout->addWidget(completionLabel, 1,1);

    ui_completionLabel = new QLabel("-- %", this);
    detailsLayout->addWidget(ui_completionLabel, 1, 2);

    ui_remainingWorkLabel = new QLabel("-- days (done) / -- days (total)");
    detailsLayout->addWidget(ui_remainingWorkLabel, 2, 2);

    QLabel *latenessLabel = new QLabel("Estimated lateness: ", this);
    detailsLayout->addWidget(latenessLabel, 3,1);

    ui_latenessLabel = new QLabel("-- %", this);
    detailsLayout->addWidget(ui_latenessLabel, 3, 2);

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
