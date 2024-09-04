#include "ramjsonobjecteditwidget.h"

#include <QRegExpValidator>

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
    ui_tabWidget->setTabToolTip(0, tr("Properties"));

    ui_mainLayout = DuUI::addBoxLayout(Qt::Vertical, mainWidget);

    ui_attributesLayout = DuUI::createFormLayout();
    DuUI::addBlock(ui_attributesLayout, ui_mainLayout);

    ui_nameEdit = new DuLineEdit(mainWidget);
    // Name validator
    QRegExp rxn = RegExUtils::getRegExp("name");
    QValidator *nameValidator = new QRegExpValidator(rxn, this);
    ui_nameEdit->setValidator(nameValidator);

    ui_attributesLayout->addRow(tr("Name"), ui_nameEdit);

    ui_shortNameEdit = new DuLineEdit(mainWidget);
    // Short Name validator
    QRegExp rxsn = RegExUtils::getRegExp("shortname");
    QValidator *shortNameValidator = new QRegExpValidator(rxsn, this);
    ui_shortNameEdit->setValidator(shortNameValidator);

    ui_attributesLayout->addRow(tr("ID"), ui_shortNameEdit);

    ui_commentEdit = new DuRichTextEdit(mainWidget);
    ui_commentEdit->setMaximumHeight(80);
    ui_attributesLayout->addRow(tr("Comment"), ui_commentEdit);

    ui_colorSelector = new DuColorSelector(this);
    ui_attributesLayout->addRow(tr("Color"), ui_colorSelector);

    ui_mainLayout->addStretch(1);
}

void RamJsonObjectEditWidget::connectEvents()
{
    connect(ui_shortNameEdit, &DuLineEdit::editingFinished,
            this, [this] () {  emit dataChanged(data()); });
    connect(ui_nameEdit, &DuLineEdit::editingFinished,
            this, [this] () {  emit dataChanged(data()); });
    connect(ui_commentEdit, &DuRichTextEdit::editingFinished,
            this, [this] () {  emit dataChanged(data()); });
    connect(ui_colorSelector, &DuColorSelector::colorChanged,
            this, [this] () {  emit dataChanged(data()); });
}

void RamJsonObjectEditWidget::setRamObjectData(const QJsonObject &obj)
{
    ui_nameEdit->setText(obj.value(RamAbstractObject::KEY_Name).toString(RamAbstractObject::DEFAULT_Name));
    ui_shortNameEdit->setText(obj.value(RamAbstractObject::KEY_ShortName).toString(RamAbstractObject::DEFAULT_ShortName));
    ui_commentEdit->setMarkdown(obj.value(RamAbstractObject::KEY_Comment).toString());
    ui_colorSelector->setColor(obj.value(RamAbstractObject::KEY_Color).toString(RamAbstractObject::DEFAULT_Color));
}

QJsonObject RamJsonObjectEditWidget::ramObjectData() const
{
    QJsonObject obj;

    obj.insert(RamAbstractObject::KEY_Name, ui_nameEdit->text());
    obj.insert(RamAbstractObject::KEY_ShortName, ui_shortNameEdit->text());
    obj.insert(RamAbstractObject::KEY_Comment, ui_commentEdit->toMarkdown());
    obj.insert(RamAbstractObject::KEY_Color, ui_colorSelector->color().name());

    return obj;
}
