#include "scheduleentryeditwidget.h"

#include "ramstep.h"

ScheduleEntryEditWidget::ScheduleEntryEditWidget(RamScheduleEntry *entry, QWidget *parent) :
    ObjectEditWidget(parent),
    m_entry(entry)
{
    setupUi();
    connectEvents();

    setObject(entry);
}

void ScheduleEntryEditWidget::reInit(RamObject *o)
{
    m_entry = qobject_cast<RamScheduleEntry*>(o);

    QSignalBlocker b1(ui_titleEdit);
    QSignalBlocker b2(ui_colorSelector);
    QSignalBlocker b3(ui_stepBox);
    QSignalBlocker b4(ui_dateEdit);
    QSignalBlocker b5(ui_rowBox);
    QSignalBlocker b6(ui_commentEdit);

    this->setEnabled(false);

    if (m_entry) {
        RamProject *project = m_entry->project();

        ui_titleEdit->setText(m_entry->name());
        ui_colorSelector->setColor(m_entry->color());
        ui_dateEdit->setDate(m_entry->date());
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
        ui_commentEdit->setPlainText(m_entry->comment());
#else
        ui_commentEdit->setMarkdown(m_entry->comment());
#endif

        if (project) {

            RamStep *step = m_entry->step();

            ui_stepBox->setObjectModel(project->steps(), tr("Steps"));
            ui_stepBox->setObject(step);

            ui_rowBox->setObjectModel(project->scheduleRows());
            ui_rowBox->setObject(m_entry->row());

            ui_titleEdit->setEnabled(!step);
            ui_colorSelector->setEnabled(!step);

            this->setEnabled(true);
        }
        else {
            ui_rowBox->setObjectModel(nullptr);
            ui_stepBox->setObjectModel(nullptr);
        }
    }
    else {
        ui_titleEdit->setText("");
        ui_colorSelector->setColor(QColor());
        ui_dateEdit->setDate(QDate());
        ui_commentEdit->setPlainText("");
        ui_rowBox->setObjectModel(nullptr);
        ui_stepBox->setObjectModel(nullptr);
    }
}

void ScheduleEntryEditWidget::setupUi()
{
    hideName();
    ui_attributesWidget->hide();

    auto formLayout = new QFormLayout();
    formLayout->setSpacing(3);
    formLayout->setContentsMargins(3,3,3,3);
    ui_mainLayout->addLayout(formLayout);

    ui_titleEdit = new DuLineEdit(this);
    ui_titleEdit->setPlaceholderText(tr("Short Title"));
    formLayout->addRow(tr("Title"), ui_titleEdit);

    ui_colorSelector = new DuQFColorSelector();
    formLayout->addRow(tr("Color"), ui_colorSelector);

    ui_stepBox = new RamObjectComboBox(this);
    formLayout->addRow(tr("Step"), ui_stepBox);

    ui_dateEdit = new QDateEdit(this);
    ui_dateEdit->setCalendarPopup(true);
    formLayout->addRow(tr("Date"), ui_dateEdit);

    ui_rowBox = new RamObjectComboBox(this);
    formLayout->addRow(tr("Row"), ui_rowBox);

    ui_commentEdit = new DuQFTextEdit(this);
    ui_commentEdit->setProperty("class", "duBlock");
    ui_commentEdit->setPlaceholderText("Comment...");
    ui_commentEdit->setMinimumHeight(30);
    ui_commentEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui_commentEdit->setObjectName("commentEdit");
    ui_mainLayout->addWidget(ui_commentEdit);

    /*
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
    }*/
}

void ScheduleEntryEditWidget::connectEvents()
{
    connect(ui_titleEdit, &DuLineEdit::editingFinished,
            this, [this] () {
        if (m_entry) m_entry->setName(ui_titleEdit->text());
    });

    connect(ui_colorSelector, &DuQFColorSelector::colorChanged,
            this, [this] (const QColor &c) {
                if (m_entry) m_entry->setColor(c);
            });

    connect(ui_stepBox, &RamObjectComboBox::currentObjectChanged,
            this, [this] (RamObject *o) {
                if (m_entry) m_entry->setStep(o);
                ui_titleEdit->setEnabled(!o);
                ui_colorSelector->setEnabled(!o);
            });

    connect(ui_dateEdit, &QDateEdit::dateChanged,
            this, [this] (const QDate &date) {
                if (m_entry) m_entry->setDate(date);
            });

    connect(ui_rowBox, &RamObjectComboBox::currentObjectChanged,
            this, [this] (RamObject *o) {
                if (m_entry) m_entry->setRow(o);
            });

    connect( ui_commentEdit, &DuQFTextEdit::editingFinished,
            this, [this] () {
        if (m_entry)
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
            m_entry->setComment(ui_commentEdit->toPlainText());
#else
            m_entry->setComment(ui_commentEdit->toMarkdown());
#endif
    });
}
