#include "ramjsonobjecteditwidget.h"

#include <QRegularExpressionValidator>

#include "duapp/duui.h"
#include "duutils/utils.h"
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

    QWidget *mainWidget = new QWidget(this);
    mainWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui_tabWidget->addTab(mainWidget, DuIcon(":/icons/status"), "");
    ui_tabWidget->setTabToolTip(0, tr("General properties"));

    ui_mainLayout = DuUI::addBoxLayout(Qt::Vertical, mainWidget);

    ui_mainLayout->addWidget(new QLabel("<b>" +
                                        tr("General properties") +
                                        "</b>"));

    ui_attributesLayout = DuUI::createFormLayout();
    DuUI::addBlock(ui_attributesLayout, ui_mainLayout);

    ui_nameEdit = new DuLineEdit(mainWidget);
    // Name validator
    QRegularExpression rxn = RegExUtils::getRegularExpression("name");
    QValidator *nameValidator = new QRegularExpressionValidator(rxn, this);
    ui_nameEdit->setValidator(nameValidator);

    ui_attributesLayout->addRow(tr("Name"), ui_nameEdit);

    ui_shortNameEdit = new DuLineEdit(mainWidget);
    // Short Name validator
    QRegularExpression rxsn = RegExUtils::getRegularExpression("shortname");
    QValidator *shortNameValidator = new QRegularExpressionValidator(rxsn, this);
    ui_shortNameEdit->setValidator(shortNameValidator);

    ui_attributesLayout->addRow(tr("ID"), ui_shortNameEdit);

    ui_commentEdit = new DuRichTextEdit(mainWidget);
    ui_commentEdit->setMaximumHeight(80);
    ui_attributesLayout->addRow(tr("Comment"), ui_commentEdit);

    ui_colorSelector = new DuColorSelector(this);
    ui_attributesLayout->addRow(tr("Color"), ui_colorSelector);

    ui_mainLayout->addStretch(1);

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
    connect(ui_shortNameEdit, &DuLineEdit::editingFinished,
            this, &RamJsonObjectEditWidget::emitDataChanged);
    connect(ui_nameEdit, &DuLineEdit::editingFinished,
            this, &RamJsonObjectEditWidget::emitDataChanged);
    connect(ui_commentEdit, &DuRichTextEdit::editingFinished,
            this, &RamJsonObjectEditWidget::emitDataChanged);
    connect(ui_colorSelector, &DuColorSelector::colorChanged,
            this, &RamJsonObjectEditWidget::emitDataChanged);

    connect(ui_customSettingsEdit, &DuRichTextEdit::editingFinished,
            this, &RamJsonObjectEditWidget::emitDataChanged);
}

void RamJsonObjectEditWidget::setRamObjectData(const QJsonObject &obj)
{
    ui_nameEdit->setText(obj.value(RamAbstractObject::KEY_Name).toString(RamAbstractObject::DEFAULT_Name));
    ui_shortNameEdit->setText(obj.value(RamAbstractObject::KEY_ShortName).toString(RamAbstractObject::DEFAULT_ShortName));
    ui_commentEdit->setMarkdown(obj.value(RamAbstractObject::KEY_Comment).toString());
    ui_colorSelector->setColor(obj.value(RamAbstractObject::KEY_Color).toString(RamAbstractObject::DEFAULT_Color));
    ui_customSettingsEdit->setText(obj.value(RamAbstractObject::KEY_CustomSettings).toString( ));
}

QJsonObject RamJsonObjectEditWidget::ramObjectData() const
{
    QJsonObject obj;

    obj.insert(RamAbstractObject::KEY_Name, ui_nameEdit->text());
    obj.insert(RamAbstractObject::KEY_ShortName, ui_shortNameEdit->text());
    obj.insert(RamAbstractObject::KEY_Comment, ui_commentEdit->toMarkdown());
    obj.insert(RamAbstractObject::KEY_Color, ui_colorSelector->color().name());
    obj.insert(RamAbstractObject::KEY_CustomSettings, ui_customSettingsEdit->toPlainText());

    return obj;
}

void RamJsonObjectEditWidget::emitDataChanged()
{
    emit dataChanged(data());
}
