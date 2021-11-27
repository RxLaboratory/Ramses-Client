#include "installpage.h"

InstallPage::InstallPage(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);

    ui_folderSelector = new DuQFFolderSelectorWidget(DuQFFolderSelectorWidget::Folder, this);
    ui_mainFolderLayout->addWidget(ui_folderSelector);

    ui_folderSelector->setPath(m_settings.value("ramsesPath", QDir::homePath() + "/Ramses").toString());

    ui_serverAddressEdit->setText(m_settings.value("server/address", "localhost/ramses/").toString());
    ui_sslBox->setChecked( m_settings.value("server/ssl", true).toBool() );

    connect(ui_serverAddressEdit, SIGNAL(editingFinished()), this, SLOT(serverAddressEdit_editingFinished()));
    connect(ui_sslBox, SIGNAL(clicked(bool)), this, SLOT(sslCheckBox_clicked(bool)));
    connect(ui_folderSelector, &DuQFFolderSelectorWidget::pathChanged, this, &InstallPage::setRamsesPath);
    connect(ui_helpButton, SIGNAL(clicked()), this, SLOT(help()));
    connect(ui_loginButton, &QPushButton::clicked, this, &InstallPage::login_clicked);
}

void InstallPage::help()
{
    QDesktopServices::openUrl ( QUrl( URL_DOC ) );
}

void InstallPage::login_clicked()
{
    if ( ui_serverAddressEdit->text() == "" || ui_serverAddressEdit->text() == "/" )
    {
        m_settings.setValue("server/address", "localhost/");
    }
    emit login();
}

void InstallPage::setRamsesPath(QString p)
{
    m_settings.setValue("ramsesPath", p);
    Ramses::instance()->setRamsesPath(p);
}

void InstallPage::serverAddressEdit_editingFinished()
{
    QString address = ui_serverAddressEdit->text();
    if (!address.endsWith("/"))
    {
        address += "/";
        ui_serverAddressEdit->setText(address);
    }
    m_settings.setValue("server/address", address);
}

void InstallPage::sslCheckBox_clicked(bool checked)
{
    m_settings.setValue("server/ssl", checked);
}
