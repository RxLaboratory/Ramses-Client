#include "serversettingswidget.h"

ServerSettingsWidget::ServerSettingsWidget(QWidget *parent) :
    QWidget(parent)
{
    setupUi();

    ui_sslCheckBox->setChecked( m_settings.value("server/ssl", true).toBool() );
    ui_updateFreqSpinBox->setValue( m_settings.value("server/updateFreq", 2).toInt());
    ui_timeoutSpinBox->setValue( m_settings.value("server/timeout", 3000).toInt()/1000 );
    _app = (DuApplication *)qApp;
    _app->setIdleTimeOut( m_settings.value("server/updateFreq", 2).toInt()*60*1000 );

    ui_logoutWidget->hide();

    connectEvents();
}

void ServerSettingsWidget::sslCheckBox_clicked(bool checked)
{
    m_settings.setValue("server/ssl", checked);
}

void ServerSettingsWidget::updateFreqSpinBox_editingFinished()
{
    int to = ui_updateFreqSpinBox->value();
    m_settings.setValue("server/updateFreq", to );
    _app->setIdleTimeOut( to*60*1000 );
}

void ServerSettingsWidget::timeoutSpinBox_editingFinished()
{
    int to = ui_timeoutSpinBox->value();
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
    ui_logoutWidget->setVisible(online);
    ui_settingsWidget->setEnabled(!online);
}

void ServerSettingsWidget::setupUi()
{
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0,9,0,0);

    mainLayout->addStretch();

    QVBoxLayout *layout = new QVBoxLayout();
    layout->setSpacing(3);
    layout->setContentsMargins(0,0,0,0);
    mainLayout->addLayout(layout);

    layout->addStretch();

    ui_logoutWidget = new QWidget(this);

    QVBoxLayout *logoutLayout = new QVBoxLayout(ui_logoutWidget);
    logoutLayout->setSpacing(3);
    logoutLayout->setContentsMargins(0,0,0,0);
    layout->addWidget(ui_logoutWidget);

    QLabel *logoutLabel = new QLabel("You must log out to change the network settings", this);
    logoutLayout->addWidget(logoutLabel);

    ui_logoutButton = new QPushButton("Logout", this);
    logoutLayout->addWidget(ui_logoutButton);

    ui_settingsWidget = new QWidget(this);

    QFormLayout *settingsLayout = new QFormLayout(ui_settingsWidget);
    settingsLayout->setHorizontalSpacing(3);
    settingsLayout->setVerticalSpacing(3);
    layout->addWidget(ui_settingsWidget);

    QLabel *adressLabel = new QLabel("Server Address", this);
    settingsLayout->setWidget(0, QFormLayout::LabelRole, adressLabel);

    QHBoxLayout *adressLayout = new QHBoxLayout();
    adressLayout->setSpacing(3);
    adressLayout->setContentsMargins(0,0,0,0);
    settingsLayout->setLayout(0, QFormLayout::FieldRole, adressLayout);

    QLabel *protocolLabel = new QLabel("http(s)://", this);
    adressLayout->addWidget(protocolLabel);

    ui_serverAddressBox = new DuQFServerComboBox("localhost/ramses/", this);
    adressLayout->addWidget(ui_serverAddressBox);

    QLabel *sslLabel = new QLabel("Secure connexion", this);
    settingsLayout->setWidget(1, QFormLayout::LabelRole, sslLabel);

    ui_sslCheckBox = new QCheckBox("Use SSL", this);
    settingsLayout->setWidget(1, QFormLayout::FieldRole, ui_sslCheckBox);

    QLabel *updateFreqLabel = new QLabel("Update every", this);
    settingsLayout->setWidget(2, QFormLayout::LabelRole, updateFreqLabel);

    ui_updateFreqSpinBox = new QSpinBox(this);
    ui_updateFreqSpinBox->setMinimum(1);
    ui_updateFreqSpinBox->setMaximum(10);
    ui_updateFreqSpinBox->setValue(2);
    ui_updateFreqSpinBox->setSuffix(" minutes");
    settingsLayout->setWidget(2, QFormLayout::FieldRole, ui_updateFreqSpinBox);

    QLabel *timeOutLabel = new QLabel("Server timeout", this);
    settingsLayout->setWidget(3, QFormLayout::LabelRole, timeOutLabel);

    ui_timeoutSpinBox = new QSpinBox(this);
    ui_timeoutSpinBox->setMinimum(1);
    ui_timeoutSpinBox->setMaximum(10);
    ui_timeoutSpinBox->setValue(3);
    ui_timeoutSpinBox->setSuffix(" seconds");

    layout->addStretch();

    mainLayout->addStretch();
}

void ServerSettingsWidget::connectEvents()
{

    connect(ui_sslCheckBox, SIGNAL(clicked(bool)), this, SLOT(sslCheckBox_clicked(bool)));
    connect(ui_logoutButton, SIGNAL(clicked()), this, SLOT(logout()));
    connect(ui_updateFreqSpinBox, SIGNAL(editingFinished()), this, SLOT( updateFreqSpinBox_editingFinished()));
    connect(ui_timeoutSpinBox, SIGNAL(editingFinished()), this, SLOT( timeoutSpinBox_editingFinished()));
    connect(DBInterface::instance(), &DBInterface::connectionStatusChanged, this, &ServerSettingsWidget::dbiConnectionStatusChanged);
    connect(DBInterface::instance(), SIGNAL(serverAddressChanged(QString)), ui_serverAddressBox, SLOT(setAddress(QString)));
}

