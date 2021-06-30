#include "scheduleentryeditwidget.h"

ScheduleEntryEditWidget::ScheduleEntryEditWidget(RamScheduleEntry *entry, QWidget *parent):
    ObjectEditWidget(entry, parent)
{
    setupUi();
    setObject(entry);
    connectEvents();
}

void ScheduleEntryEditWidget::setObject(RamObject *obj)
{
    RamScheduleEntry *entry = qobject_cast<RamScheduleEntry*>( obj );
    this->setEnabled(false);

    ObjectEditWidget::setObject( obj );
    m_entry = entry;

    QSignalBlocker b1(ui_userBox);
    QSignalBlocker b2(ui_stepBox);
    QSignalBlocker b3(ui_dateEdit);

    ui_userBox->setCurrentIndex(-1);
    ui_stepBox->setCurrentIndex(-1);
    ui_dateEdit->setDate(QDate::currentDate());

    if (!m_entry) return;

    ui_userBox->setObject( m_entry->user() );
    ui_stepBox->setObject( m_entry->step() );
    ui_dateEdit->setDate( m_entry->date().date() );

    this->setEnabled( Ramses::instance()->isLead() );
}

void ScheduleEntryEditWidget::update()
{
    if (m_entry) return;
    if (!checkInput()) return;
    updating = true;

    RamUser *user = qobject_cast<RamUser*>( ui_userBox->currentObject() );
    if (user) m_entry->setUser( user );

    RamStep *step = qobject_cast<RamStep*>( ui_stepBox->currentObject() );
    if (step) m_entry->setStep( step );

    QDateTime date = ui_dateEdit->date().startOfDay();
    if (ui_ampmBox->currentIndex() == 1) date.setTime(QTime(12,0));

    ObjectEditWidget::update();

    updating = false;
}

void ScheduleEntryEditWidget::setupUi()
{
    QLabel *userLabel = new QLabel("User", this);
    ui_mainFormLayout->addWidget(userLabel, 3, 0);

    ui_userBox = new RamObjectListComboBox(Ramses::instance()->users(), this);
    ui_mainFormLayout->addWidget(ui_userBox, 3, 1);

    QLabel *stepLabel = new QLabel("Step", this);
    ui_mainFormLayout->addWidget(stepLabel, 4, 0);

    ui_stepBox = new RamObjectListComboBox(this);
    ui_mainFormLayout->addWidget(ui_stepBox, 4, 1);

    QLabel *dateLabel = new QLabel("Date", this);
    ui_mainFormLayout->addWidget(dateLabel, 5, 0);

    QWidget *dateWidget = new QWidget(this);
    QHBoxLayout *dateLayout = new QHBoxLayout(dateWidget);
    dateLayout->setContentsMargins(0,0,0,0);
    dateLayout->setSpacing(3);

    ui_dateEdit = new QDateEdit(this);
    ui_dateEdit->setCalendarPopup(true);
    ui_dateEdit->setDate(QDate::currentDate());
    dateLayout->addWidget(ui_dateEdit);

    ui_ampmBox = new QComboBox(this);
    ui_ampmBox->addItem("AM");
    ui_ampmBox->addItem("PM");
    dateLayout->addWidget(ui_ampmBox);

    ui_mainFormLayout->addWidget( dateWidget, 5, 1);
}

void ScheduleEntryEditWidget::connectEvents()
{
    connect(ui_userBox, SIGNAL(currentObjectChanged(RamObject*)), this, SLOT(update()));
    connect(ui_stepBox, SIGNAL(currentObjectChanged(RamObject*)), this, SLOT(update()));
    connect(ui_dateEdit, SIGNAL(dateChanged(QDate)), this, SLOT(update()));
    connect(ui_ampmBox, SIGNAL(currentIndexChanged(int)), this, SLOT(update()));
}
