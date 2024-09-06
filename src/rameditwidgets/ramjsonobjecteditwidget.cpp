#include "ramjsonobjecteditwidget.h"

#include <QRegularExpressionValidator>

#include "duapp/duui.h"
#include "duwidgets/duicon.h"

RamJsonObjectEditWidget::RamJsonObjectEditWidget(QWidget *parent) :
    DuScrollArea(parent)
{
    setupUi();
    connectEvents();
    // Set default data
    setRamObjectData(QJsonObject());
}

QJsonObject RamJsonObjectEditWidget::data() const
{
    return ramObjectData();
}

void RamJsonObjectEditWidget::setData(const QJsonObject &obj)
{
    setRamObjectData(obj);
    emit dataChanged(data());
}

void RamJsonObjectEditWidget::setupUi()
{
    ui_tabWidget = new DuTabWidget(this);
    this->setWidget(ui_tabWidget);
    this->setWidgetResizable(true);
    this->setMinimumWidth(300);

    ui_propertiesWidget = new RamObjectPropertiesWidget(this);

    ui_tabWidget->addTab(ui_propertiesWidget, DuIcon(":/icons/status"), "");
    ui_tabWidget->setTabToolTip(0, tr("General properties"));

    auto customWidget = new QWidget(this);
    ui_tabWidget->addTab(customWidget, DuIcon(":/icons/settings"), "");
    ui_tabWidget->setTabToolTip(1, tr("Custom settings"));

    auto customLayout = DuUI::addBoxLayout(Qt::Vertical, customWidget);

    customLayout->addWidget(new QLabel(
        "<b>"+tr("Custom settings")+"</b>"
        ));

    ui_customSettingsEdit = new DuRichTextEdit(ui_tabWidget);
    ui_customSettingsEdit->setUseMarkdown(false);
    ui_customSettingsEdit->setPlaceholderText(tr("General settings"));
    customLayout->addWidget(ui_customSettingsEdit);
}

void RamJsonObjectEditWidget::connectEvents()
{
    connect(ui_propertiesWidget, &RamObjectPropertiesWidget::edited,
            this, &RamJsonObjectEditWidget::emitDataChanged);

    connect(ui_customSettingsEdit, &DuRichTextEdit::editingFinished,
            this, &RamJsonObjectEditWidget::emitDataChanged);
}

void RamJsonObjectEditWidget::setRamObjectData(const QJsonObject &obj)
{
    ui_propertiesWidget->setName(obj.value(RamAbstractObject::KEY_Name).toString());
    ui_propertiesWidget->setShortName(obj.value(RamAbstractObject::KEY_ShortName).toString());
    ui_propertiesWidget->setComment(obj.value(RamAbstractObject::KEY_Comment).toString());
    ui_propertiesWidget->setColor(QColor(
        obj.value(RamAbstractObject::KEY_Color).toString(RamAbstractObject::DEFAULT_Color)
        ));

    ui_customSettingsEdit->setText(obj.value(RamAbstractObject::KEY_CustomSettings).toString( ));
}

QJsonObject RamJsonObjectEditWidget::ramObjectData() const
{
    QJsonObject obj;

    obj.insert(RamAbstractObject::KEY_Name, ui_propertiesWidget->name());
    obj.insert(RamAbstractObject::KEY_ShortName, ui_propertiesWidget->shortName());
    obj.insert(RamAbstractObject::KEY_Comment, ui_propertiesWidget->comment());
    obj.insert(RamAbstractObject::KEY_Color, ui_propertiesWidget->color().name());

    obj.insert(RamAbstractObject::KEY_CustomSettings, ui_customSettingsEdit->toPlainText());

    return obj;
}

void RamJsonObjectEditWidget::emitDataChanged()
{
    emit dataChanged(data());
}
