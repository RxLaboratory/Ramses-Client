#include "ramobjectpropertieswizardpage.h"

#include <QMessageBox>

#include "duapp/duui.h"

RamObjectPropertiesWizardPage::RamObjectPropertiesWizardPage(QWidget *parent):
    QWizardPage(parent)
{
    setupUi();
    connectEvents();
}

bool RamObjectPropertiesWizardPage::validatePage()
{
    if (ui_propertiesWidget->name() == "") {
        QMessageBox::information(this,
                                 tr("Missing name"),
                                 tr("The project name is empty.")
                                 );
        return false;
    }

    if (ui_propertiesWidget->shortName() == "") {
        QMessageBox::information(this,
                                 tr("Missing ID"),
                                 tr("The ID must be set.")
                                 );
        return false;
    }

    return true;
}

void RamObjectPropertiesWizardPage::setupUi()
{
    this->setTitle(tr("Project details"));
    this->setSubTitle(tr("Set the project name and other details."));

    auto layout = DuUI::createBoxLayout(Qt::Vertical);
    DuUI::centerLayout(layout, this);

    ui_propertiesWidget = new RamObjectPropertiesWidget(this);
    ui_propertiesWidget->showFolder(false);
    layout->addWidget(ui_propertiesWidget);

    registerField("name", ui_propertiesWidget, "name", "nameChanged");
    registerField("shortName", ui_propertiesWidget, "shortName", "shortNameChanged");
}

void RamObjectPropertiesWizardPage::connectEvents()
{

}
