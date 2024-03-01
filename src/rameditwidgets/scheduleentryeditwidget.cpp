#include "scheduleentryeditwidget.h"

#include "ramstatustablemodel.h"

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
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
    m_entry->setComment(ui_commentEdit->toPlainText());
#else
    m_entry->setComment(ui_commentEdit->toMarkdown());
#endif
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
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
        ui_commentEdit->setPlainText(m_entry->comment());
#else
        ui_commentEdit->setMarkdown(m_entry->comment());
#endif

    // Get due tasks
    const QDate date = m_date.date();
    const QString uUuid = m_user->uuid();
    QSet<RamStatus*> tasks = m_project->assetStatus()->getStatus(date, uUuid);
    tasks += m_project->shotStatus()->getStatus(date, uUuid);

    if (!tasks.isEmpty()) {
        dummyLayout->addWidget(new QLabel( tr("Due tasks:") ));

        ui_dueTasksList = new RamObjectView(RamObjectView::List, mainWidget);
        ui_dueTasksList->setProperty("class", "duBlock");
        ui_dueTasksList->verticalHeader()->hide();
        dummyLayout->addWidget(ui_dueTasksList);

        auto model = new RamObjectModel(RamAbstractObject::Status, this);
        for (const auto task: qAsConst(tasks)) {
            model->appendObject(task->uuid());
        }
        ui_dueTasksList->setObjectModel(model);
        ui_dueTasksList->setEditableObjects(true, RamUser::Lead);

        ui_dueTasksList->verticalHeader()->resizeSections(QHeaderView::ResizeToContents);
    }
}

void ScheduleEntryEditWidget::connectEvents()
{
    connect(ui_stepBox, &RamObjectComboBox::currentObjectChanged,
            this, &ScheduleEntryEditWidget::setStep);

    connect( ui_commentEdit, &DuQFTextEdit::editingFinished,
            this, &ScheduleEntryEditWidget::setComment);
}
