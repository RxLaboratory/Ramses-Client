#include "serversettingswidget.h"

ServerSettingsWidget::ServerSettingsWidget(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);

    serverAddressEdit->setText(m_settings.value("server/address", "localhost/ramses/").toString());
    sslCheckBox->setChecked( m_settings.value("server/ssl", true).toBool() );
    updateFreqSpinBox->setValue( m_settings.value("server/updateFreq", 2).toInt());
    timeoutSpinBox->setValue( m_settings.value("server/timeout", 3000).toInt()/1000 );
    _app = (DuApplication *)qApp;
    _app->setIdleTimeOut( m_settings.value("server/updateFreq", 2).toInt()*60*1000 );

    logoutWidget->hide();

    connect(serverAddressEdit, SIGNAL(editingFinished()), this, SLOT(serverAddressEdit_edingFinished()));
    connect(sslCheckBox, SIGNAL(clicked(bool)), this, SLOT(sslCheckBox_clicked(bool)));
    connect(logoutButton, SIGNAL(clicked()), this, SLOT(logout()));
    connect(updateFreqSpinBox, SIGNAL(editingFinished()), this, SLOT( updateFreqSpinBox_editingFinished()));
    connect(timeoutSpinBox, SIGNAL(editingFinished()), this, SLOT( timeoutSpinBox_editingFinished()));
    connect(DBInterface::instance(), &DBInterface::connectionStatusChanged, this, &ServerSettingsWidget::dbiConnectionStatusChanged);
}

void ServerSettingsWidget::serverAddressEdit_edingFinished()
{
    QString address = serverAddressEdit->text();
    if (!address.endsWith("/"))
    {
        address += "/";
        serverAddressEdit->setText(address);
    }
    m_settings.setValue("server/address", address);
}

void ServerSettingsWidget::sslCheckBox_clicked(bool checked)
{
    m_settings.setValue("server/ssl", checked);
}

void ServerSettingsWidget::updateFreqSpinBox_editingFinished()
{
    int to = updateFreqSpinBox->value();
    m_settings.setValue("server/updateFreq", to );
    _app->setIdleTimeOut( to*60*1000 );
}

void ServerSettingsWidget::timeoutSpinBox_editingFinished()
{
    int to = timeoutSpinBox->value();
    m_settings.setValue("server/timeout", to*1000 );
}

void ServerSettingsWidget::logout()
{
    Ramses::instance()->logout();
    DBInterface::instance()->setOffline();
}

void ServerSettingsWidget::dbiConnectionStatusChanged(NetworkUtils::NetworkStatus s)
{
    bool online = s == NetworkUtils::Online;
    logoutWidget->setVisible(online);
    settingsWidget->setEnabled(!online);
}

