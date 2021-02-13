#include "serversettingswidget.h"

ServerSettingsWidget::ServerSettingsWidget(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);

    serverAddressEdit->setText(settings.value("server/address", "localhost/ramses/").toString());
    sslCheckBox->setChecked( settings.value("server/ssl", true).toBool() );

    connect(serverAddressEdit, SIGNAL(editingFinished()), this, SLOT(serverAddressEdit_edingFinished()));
    connect(sslCheckBox, SIGNAL(clicked(bool)), this, SLOT(sslCheckBox_clicked(bool)));
}

void ServerSettingsWidget::serverAddressEdit_edingFinished()
{
    QString address = serverAddressEdit->text();
    if (!address.endsWith("/"))
    {
        address += "/";
        serverAddressEdit->setText(address);
    }
    settings.setValue("server/address", address);
}

void ServerSettingsWidget::sslCheckBox_clicked(bool checked)
{
    settings.setValue("server/ssl", checked);
}

