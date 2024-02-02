#include "scheduleentryeditwidget.h"

ScheduleEntryEditWidget::ScheduleEntryEditWidget(RamProject *project, RamUser *user, const QDateTime &date, RamScheduleEntry *entry, QWidget *parent) :
    DuScrollArea(parent),
    m_user(user),
    m_project(project),
    m_date(date),
    m_entry(entry)
{
    setupUi();
    connectEvents();
}

void ScheduleEntryEditWidget::setStep(RamObject *stepObj)
{
    auto step = qobject_cast<RamStep*>(stepObj);
    if (!m_entry)
        m_entry = new RamScheduleEntry(m_user, step, m_date);
    else
        m_entry->setStep(step);
}

void ScheduleEntryEditWidget::setComment()
{
    if (!m_entry) {
        auto step = qobject_cast<RamStep*>(ui_stepBox->currentObject());
        m_entry = new RamScheduleEntry(m_user, step, m_date);
    }

    m_entry->setComment(ui_commentEdit->toMarkdown());
}

void ScheduleEntryEditWidget::setupUi()
{
    auto mainWidget = new QWidget(this);
    mainWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->setWidget(mainWidget);
    this->setWidgetResizable(true);
    this->setFrameStyle(QFrame::NoFrame);

    auto *dummyLayout = new QVBoxLayout(mainWidget);
    dummyLayout->setContentsMargins(3,3,3,3);
    dummyLayout->setSpacing(3);

    auto mainLayout = new QFormLayout();
    mainLayout->setSpacing(3);
    mainLayout->setContentsMargins(0,0,0,0);
    dummyLayout->addLayout(mainLayout);

    ui_userLabel = new QLabel("<b>" + m_user->name() + "</b>", mainWidget);
    mainLayout->addRow("User", ui_userLabel);

    QString ampm;
    if ( m_date.time().hour() >= 12 ) ampm = "pm";
    else ampm = "am";
    ui_dateLabel = new QLabel(
        "<b>" + m_date.toString("yyyy-MM-dd ") + ampm + "</b>"
        );
    mainLayout->addRow("Date", ui_dateLabel );

    ui_stepBox = new RamObjectComboBox(mainWidget);
    ui_stepBox->setObjectModel(m_project->steps());
    mainLayout->addRow("Step", ui_stepBox);
    if (m_entry)
        ui_stepBox->setObject(m_entry->step());

    ui_commentEdit = new DuQFTextEdit(mainWidget);
    ui_commentEdit->setProperty("class", "duBlock");
    ui_commentEdit->setMinimumHeight(30);
    ui_commentEdit->setPlaceholderText(tr("Comment..."));
    ui_commentEdit->setObjectName("commentEdit");
    ui_commentEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    dummyLayout->addWidget(ui_commentEdit);
    if (m_entry)
        ui_commentEdit->setMarkdown(m_entry->comment());

    // TODO List tasks due date
}

void ScheduleEntryEditWidget::connectEvents()
{
    connect(ui_stepBox, &RamObjectComboBox::currentObjectChanged,
            this, &ScheduleEntryEditWidget::setStep);

    connect( ui_commentEdit, &DuQFTextEdit::editingFinished,
            this, &ScheduleEntryEditWidget::setComment);
}
