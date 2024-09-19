#include "ramobjectpropertieswidget.h"

#include <QMessageBox>
#include <QRegularExpressionValidator>
#include <QToolButton>

#include "duwidgets/duicon.h"
#include "duapp/duui.h"
#include "duutils/utils.h"
#include "ramnamemanager.h"

RamObjectPropertiesWidget::RamObjectPropertiesWidget(QWidget *parent)
    : QWidget{parent}
{
    setupUi();
    connectEvents();
}

void RamObjectPropertiesWidget::showFolder(bool s)
{
    ui_folderButton->setVisible(s);
    ui_folderLabel->setVisible(s);
}

void RamObjectPropertiesWidget::showIdHelp() const
{
    QMessageBox::information(const_cast<RamObjectPropertiesWidget*>(this),
                             tr("Object ID"),
                             tr("The ID is used to uniquely identify all objects in the production pipeline.\n"
                                "It is also used in file names, thus it is important to keep it short and simple.\n\n"
                                "It should be unique.\n"
                                "It can contain only number, letters, or the plus (+) and minus (-) signs,\n"
                                "it must be shorter than 10 characters,\n"
                                "and it can't start with a number.")
                             );
}

void RamObjectPropertiesWidget::setupUi()
{
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    ui_mainLayout = DuUI::addBoxLayout(Qt::Vertical, this);

    ui_mainLayout->addWidget(new QLabel("<b>" +
                                        tr("General properties") +
                                        "</b>"));

    ui_attributesLayout = DuUI::createFormLayout();
    DuUI::addBlock(ui_attributesLayout, ui_mainLayout);

    ui_nameEdit = new DuLineEdit(this);
    ui_nameEdit->setPlaceholderText(tr("Object name"));
    // Name validator
    QRegularExpression rxn = RegExUtils::getRegularExpression("name");
    QValidator *nameValidator = new QRegularExpressionValidator(rxn, this);
    ui_nameEdit->setValidator(nameValidator);

    ui_attributesLayout->addRow(tr("Name") + " *", ui_nameEdit);

    auto shortNameLabelWidget = new QWidget();
    shortNameLabelWidget->setProperty("class", "transparent");
    auto shortNameLabeLayout = DuUI::createBoxLayout(Qt::Horizontal, true, shortNameLabelWidget);
    shortNameLabeLayout->addWidget(new QLabel(tr("ID") + " *", this));

    ui_idHelpButton = new QToolButton(this);
    ui_idHelpButton->setIcon(DuIcon(":/icons/help-button"));
    ui_idHelpButton->setIconSize(QSize(9,9));
    shortNameLabeLayout->addWidget(ui_idHelpButton);

    ui_shortNameEdit = new DuLineEdit(this);
    ui_shortNameEdit->setPlaceholderText("Obj-id");
    // Short Name validator
    QRegularExpression rxsn = RegExUtils::getRegularExpression("shortname");
    QValidator *shortNameValidator = new QRegularExpressionValidator(rxsn, this);
    ui_shortNameEdit->setValidator(shortNameValidator);

    ui_attributesLayout->addRow(shortNameLabelWidget, ui_shortNameEdit);

    ui_commentEdit = new DuRichTextEdit(this);
    ui_commentEdit->setMaximumHeight(80);
    ui_attributesLayout->addRow(tr("Comment"), ui_commentEdit);

    ui_colorSelector = new DuColorSelector(this);
    ui_attributesLayout->addRow(tr("Color"), ui_colorSelector);

    ui_folderButton = new QPushButton(this);
    ui_folderButton->setIcon(DuIcon(":/icons/folder"));
    ui_folderButton->setText(tr("Open"));
    ui_folderButton->setProperty("class", "transparent");
    ui_folderLabel = new QLabel(tr("Folder"), this);
    ui_attributesLayout->addRow(ui_folderLabel, ui_folderButton);
}

void RamObjectPropertiesWidget::connectEvents()
{
    connect(ui_nameEdit, &DuLineEdit::edited,
            this, [this] () {
                if (ui_shortNameEdit->text() == "")
                    ui_shortNameEdit->setText(
                        RamNameManager::nameToShortName(ui_nameEdit->text())
                        );
                emit edited();
            });

    connect(ui_shortNameEdit, &DuLineEdit::edited,
            this, &RamObjectPropertiesWidget::edited);
    connect(ui_commentEdit, &DuRichTextEdit::editingFinished,
            this, &RamObjectPropertiesWidget::edited);
    connect(ui_colorSelector, &DuColorSelector::colorEdited,
            this, &RamObjectPropertiesWidget::edited);

    connect(ui_nameEdit, &DuLineEdit::textEdited,
            this, [this] () { emit nameChanged(ui_nameEdit->text()); });
    connect(ui_shortNameEdit, &DuLineEdit::textEdited,
            this, [this] () { emit shortNameChanged(ui_shortNameEdit->text()); });
    connect(ui_commentEdit, &DuRichTextEdit::editingFinished,
            this, [this] () { emit commentChanged(ui_commentEdit->toPlainText()); });
    connect(ui_colorSelector, &DuColorSelector::colorChanged,
            this, &RamObjectPropertiesWidget::colorChanged);

    connect(ui_idHelpButton, &QToolButton::clicked,
            this, &RamObjectPropertiesWidget::showIdHelp);

    connect(ui_folderButton, &QPushButton::clicked,
            this, &RamObjectPropertiesWidget::folderClicked);
}
