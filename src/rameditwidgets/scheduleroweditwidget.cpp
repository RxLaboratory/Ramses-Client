#include "scheduleroweditwidget.h"
#include "ramobjectmodel.h"
#include "ramses.h"

ScheduleRowEditWidget::ScheduleRowEditWidget(QWidget *parent) :
    ObjectEditWidget(parent)
{
    setupUi();
    connectEvents();
}

ScheduleRowEditWidget::ScheduleRowEditWidget(RamScheduleRow *row, QWidget *parent) :
    ObjectEditWidget(parent)
{
    setupUi();
    setObject(row);
    connectEvents();
}

RamScheduleRow *ScheduleRowEditWidget::row()
{
    return m_row;
}

void ScheduleRowEditWidget::reInit(RamObject *o)
{
    m_row = qobject_cast<RamScheduleRow*>(o);

    QSignalBlocker b1(ui_userBox);

    if (m_row) {
        RamProject *project = m_row->project();
        if (project) {
            RamObjectModel *users = project->users();
            ui_userBox->setObjectModel( users, "Users" );
            ui_userBox->setObject(m_row->user());
        }
        ui_nameEdit->setText(m_row->getData("name").toString());
    }
    else {
        ui_userBox->setObjectModel(nullptr, "users" );
        ui_userBox->setObject("");
        this->setEnabled(false);
    }

    this->setEnabled(Ramses::i()->isProjectAdmin());
}

void ScheduleRowEditWidget::setUser(RamObject *usrObj)
{
    if (!m_row) return;
    m_row->setUser(usrObj);
}

void ScheduleRowEditWidget::setupUi()
{
    this->hideShortName(true);

    ui_userBox = new RamObjectComboBox(this);
    ui_mainLayout->addWidget(ui_userBox);

    ui_mainLayout->addStretch(1);
}

void ScheduleRowEditWidget::connectEvents()
{
    connect( ui_userBox, &RamObjectComboBox::currentObjectChanged, this, &ScheduleRowEditWidget::setUser);
}
