#include "aboutdialog.h"

AboutDialog::AboutDialog(QWidget *parent) :
    DuDialog(parent)
{
    setupUi(this);
    this->layout()->addLayout(verticalLayout_2);
    versionLabel->setText(qApp->applicationName() + " | version: " + qApp->applicationVersion());
    setWindowTitle("About " + qApp->applicationName());
}
