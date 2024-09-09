#include "ramobjectpropertieswizardpage.h"

#include <QMessageBox>

#include "duapp/duui.h"

RamObjectPropertiesWizardPage::RamObjectPropertiesWizardPage(QWidget *parent):
    QWizardPage(parent)
{
    setupUi();
    connectEvents();
}

void RamObjectPropertiesWizardPage::setupUi()
{
    this->setTitle(tr("Project details"));
    this->setSubTitle(tr("Set the project name and other details."));

    auto layout = DuUI::createBoxLayout(Qt::Vertical);
    DuUI::centerLayout(layout, this);

    ui_propertiesWidget = new RamObjectPropertiesWidget(this);
    layout->addWidget(ui_propertiesWidget);

    registerField("name*", ui_propertiesWidget, "name");
    registerField("shortName*", ui_propertiesWidget, "shortName");
}

void RamObjectPropertiesWizardPage::connectEvents()
{

}
