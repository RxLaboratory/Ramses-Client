#include "serversettingswidget.h"

ServerSettingsWidget::ServerSettingsWidget(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);

    serverAddressEdit->setText(settings.value("server/address", "localhost/ramses/").toString());
    sslCheckBox->setChecked( settings.value("server/ssl", true).toBool() );
    updateFreqSpinBox->setValue( settings.value("server/updateFreq", 2).toInt());
    _app = (DuApplication *)qApp;

    logoutWidget->hide();

    connect(serverAddressEdit, SIGNAL(editingFinished()), this, SLOT(serverAddressEdit_edingFinished()));
    connect(sslCheckBox, SIGNAL(clicked(bool)), this, SLOT(sslCheckBox_clicked(bool)));
    connect(logoutButton, SIGNAL(clicked()), this, SLOT(logout()));
    connect(updateFreqSpinBox, SIGNAL(editingFinished()), this, SLOT( updateFreqSpinBox_editingFinished()));
    connect(DBInterface::instance(), &DBInterface::statusChanged, this, &ServerSettingsWidget::dbiStatusChanged);
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

void ServerSettingsWidget::updateFreqSpinBox_editingFinished()
{
    int to = updateFreqSpinBox->value();
    settings.setValue("server/updateFreq", to );
    _app->setIdleTimeOut( to*60*1000 );
}

void ServerSettingsWidget::logout()
{
    Ramses::instance()->logout();
}

void ServerSettingsWidget::dbiStatusChanged(NetworkUtils::NetworkStatus s, QString /*m*/)
{
    bool online = s == NetworkUtils::Online;
    logoutWidget->setVisible(online);
    settingsWidget->setEnabled(!online);
}

