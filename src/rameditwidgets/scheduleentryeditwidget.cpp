#include "scheduleentryeditwidget.h"

ScheduleEntryEditWidget::ScheduleEntryEditWidget(RamProject *project, RamUser *user, const QDateTime &date, QWidget *parent) :
    DuScrollArea(parent),
    m_user(user),
    m_project(project),
    m_date(date)
{
    setupUi();
    connectEvents();
}

void ScheduleEntryEditWidget::setupUi()
{
    auto mainWidget = new QWidget(this);
    mainWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->setWidget(mainWidget);
    this->setWidgetResizable(true);
    this->setFrameStyle(QFrame::NoFrame);

    auto mainLayout = new QFormLayout(mainWidget);
    mainLayout->setSpacing(3);
    mainLayout->setContentsMargins(3,3,3,3);

    ui_userLabel = new QLabel(m_user->name(), this);
    mainLayout->addRow("User", ui_userLabel);

    QString ampm;
    if ( m_date.time().hour() >= 12 ) ampm = "pm";
    else ampm = "am";
    ui_dateLabel = new QLabel(
        m_date.toString("yyyy-MM-dd " + ampm)
        );
    mainLayout->addRow("Date", ui_dateLabel );

    ui_stepBox = new RamObjectComboBox(this);
    ui_stepBox->setObjectModel(m_project->steps(), "Steps");
    mainLayout->addRow("Step", ui_stepBox);

    ui_commentEdit = new DuQFTextEdit(mainWidget);
    ui_commentEdit->setMaximumHeight(80);
    ui_commentEdit->setObjectName("commentEdit");
    mainLayout->addRow("Comment", ui_commentEdit);
}

void ScheduleEntryEditWidget::connectEvents()
{

}
