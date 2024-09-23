#include "ramjsonprojecteditwidget.h"

#include "duapp/app-config.h"
#include "duapp/duui.h"
#include "ramproject.h"
#include "ramobjectmodel.h"

RamJsonProjectEditWidget::RamJsonProjectEditWidget(const QString &uuid, QWidget *parent):
    RamJsonObjectEditWidget(uuid, parent)
{
    setupUi();
    connectEvents();
    setProject();
}

QJsonObject RamJsonProjectEditWidget::data() const
{
    QJsonObject obj = ramObjectData();
    obj.insert(RamProject::KEY_Width, ui_resolutionWidget->getWidth());
    obj.insert(RamProject::KEY_Height, ui_resolutionWidget->getHeight());
    obj.insert(RamProject::KEY_FrameRate, ui_framerateWidget->framerate());
    obj.insert(RamProject::KEY_PixelAspectRatio, ui_parBox->value());
    obj.insert(RamProject::KEY_Deadline, ui_deadlineEdit->date().toString(DATE_DATA_FORMAT));
    return obj;
}

void RamJsonProjectEditWidget::setData(const QJsonObject &obj)
{
    setRamObjectData(obj);

    ui_resolutionWidget->setWidth(
        obj.value(RamProject::KEY_Width).toInt(1920)
        );
    ui_resolutionWidget->setHeight(
        obj.value(RamProject::KEY_Height).toInt(1080)
        );

    ui_framerateWidget->setFramerate(
        obj.value(RamProject::KEY_FrameRate).toDouble(24.0)
        );

    ui_parBox->setValue(
        obj.value(RamProject::KEY_PixelAspectRatio).toDouble(1.0)
        );

    ui_deadlineEdit->setDate(
        QDate::fromString(
            obj.value(RamProject::KEY_Deadline).toString(),
            DATE_DATA_FORMAT
            )
        );

    emit dataChanged(data());
}

void RamJsonProjectEditWidget::hideUsersTab()
{
    if (!ui_userList)
        return;
    ui_tabWidget->setTabVisible(1, false);
}

void RamJsonProjectEditWidget::setProject()
{
    if (!exists()) {
        return;
    }

    _project = RamProject::get(uuid());
    if (!_project)
        return;

    ui_userList->setObjectModel(_project->users());
}

void RamJsonProjectEditWidget::setupUi()
{
    setupMainTab();
    if (exists())
        setupUsersTab();
}

void RamJsonProjectEditWidget::setupMainTab()
{
    auto mainLayout = ui_propertiesWidget->mainLayout();

    auto detailsLabel = new QLabel(
        "<b>"+tr("Technical details")+"</b>"
        );
    mainLayout->addWidget(detailsLabel);

    auto detailsLayout = DuUI::createFormLayout();
    DuUI::addBlock(detailsLayout, mainLayout);

    ui_resolutionWidget = new ResolutionWidget(this);
    detailsLayout->addRow(tr("Delivery resolution"), ui_resolutionWidget);

    ui_framerateWidget = new FramerateWidget(this);
    detailsLayout->addRow(tr("Delivery framerate"), ui_framerateWidget);

    ui_parBox = new DuDoubleSpinBox(this);
    detailsLayout->addRow(tr("Delivery pixel aspect ratio"), ui_parBox);
    ui_parBox->setMinimum(0.01);
    ui_parBox->setMaximum(10.0);
    ui_parBox->setDecimals(2);
    ui_parBox->setValue(1.0);

    ui_deadlineEdit = new DuDateEdit(this);
    ui_deadlineEdit->setCalendarPopup(true);
    ui_deadlineEdit->setDate( QDate::currentDate() );
    detailsLayout->addRow(tr("Deadline"), ui_deadlineEdit);

    mainLayout->addStretch(1);
}

void RamJsonProjectEditWidget::setupUsersTab()
{
    auto usersWidget = new QWidget(this);
    ui_tabWidget->insertTab(1, usersWidget, DuIcon(":/icons/users90"), "");
    ui_tabWidget->setTabToolTip(1, tr("Estimations"));

    auto userLayout = DuUI::addBoxLayout(Qt::Vertical, usersWidget);

    ui_userList = new ObjectListWidget(true, RamUser::Admin, this);
    ui_userList->setEditMode(ObjectListWidget::NoEdit);
    ui_userList->setTitle("Users");
    ui_userList->setSortable(true);

    userLayout->addWidget(ui_userList);
}

void RamJsonProjectEditWidget::connectEvents()
{
    connect(ui_resolutionWidget, &ResolutionWidget::resolutionEdited,
            this, &RamJsonObjectEditWidget::emitEdited);
    connect(ui_framerateWidget, &FramerateWidget::framerateEdited,
            this, &RamJsonObjectEditWidget::emitEdited);
    connect(ui_parBox, &DuDoubleSpinBox::edited,
            this, &RamJsonObjectEditWidget::emitEdited);
    connect(ui_deadlineEdit, &DuDateEdit::edited,
            this, &RamJsonObjectEditWidget::emitEdited);
}
