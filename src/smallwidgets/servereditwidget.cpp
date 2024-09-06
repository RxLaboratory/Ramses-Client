#include "servereditwidget.h"
#include "qcheckbox.h"
#include "qgridlayout.h"
#include "qlabel.h"
#include "qlineedit.h"
#include "qpushbutton.h"
#include "qurl.h"
#include <QDesktopServices>

ServerEditWidget::ServerEditWidget(QWidget *parent)
    : QWidget{parent}
{
    setupUi();
    connectEvents();
}

void ServerEditWidget::setAddress(QString a)
{
    ui_serverAddressEdit->setText(a);
}

QString ServerEditWidget::address()
{
    return ui_serverAddressEdit->text();
}

void ServerEditWidget::setSsl(bool s)
{
    ui_sslCheckBox->setChecked(s);
    updatePort(s);
}

bool ServerEditWidget::ssl() const
{
    return ui_sslCheckBox->isChecked();
}

void ServerEditWidget::setUpdateFreq(int f)
{
    ui_updateFreqSpinBox->setValue(f/1000);
}

int ServerEditWidget::updateFreq() const
{
    return ui_updateFreqSpinBox->value()*1000;
}

void ServerEditWidget::setTimeout(int t)
{
    ui_timeoutSpinBox->setValue(t/1000);
}

int ServerEditWidget::timeout() const
{
    return ui_timeoutSpinBox->value()*1000;
}

void ServerEditWidget::setPort(int p)
{
    ui_portBox->setValue(p);
}

int ServerEditWidget::port() const
{
    return ui_portBox->value();
}

void ServerEditWidget::orderServer()
{
    QDesktopServices::openUrl ( QUrl( "http://ramses.rxlab.io" ) );
}

void ServerEditWidget::updatePort(bool useSSL)
{
    if (useSSL) ui_portBox->setValue(443);
    else ui_portBox->setValue(80);
}

void ServerEditWidget::setupUi()
{
    QGridLayout *formLayout = new QGridLayout(this);
    formLayout->setAlignment(Qt::AlignTop);
    formLayout->setContentsMargins(0,0,0,0);
    formLayout->setSpacing(3);

    QLabel *adressLabel = new QLabel("Server Address", this);
    formLayout->addWidget(adressLabel, 0, 0);

    QHBoxLayout *adressLayout = new QHBoxLayout();
    adressLayout->setSpacing(3);
    adressLayout->setContentsMargins(0,0,0,0);
    formLayout->addLayout(adressLayout, 0, 1);

    QLabel *protocolLabel = new QLabel("http(s)://", this);
    adressLayout->addWidget(protocolLabel);

    ui_serverAddressEdit = new QLineEdit(this);
    ui_serverAddressEdit->setPlaceholderText("ramses.rxlab.io/yourAccountName");
    // Validator
    QRegularExpression r("^(?!https?:\\/\\/)[-a-zA-Z0-9@:%_\\+.~#?&//=]{2,256}\\.[a-z]{2,63}\\b(\\/[-a-zA-Z0-9@:%_\\+.~#?&//=]*)?$");
    QValidator *validator = new QRegularExpressionValidator(r, this);
    ui_serverAddressEdit->setValidator(validator);
    adressLayout->addWidget(ui_serverAddressEdit);

    QLabel *sslLabel = new QLabel(tr("Secure connexion"), this);
    formLayout->addWidget(sslLabel, 1, 0);

    ui_sslCheckBox = new QCheckBox("Use SSL", this);
    ui_sslCheckBox->setChecked(true);
    formLayout->addWidget(ui_sslCheckBox, 1, 1);

    QLabel *portLabel = new QLabel(tr("TCP Port"), this);
    formLayout->addWidget(portLabel, 2, 0);

    ui_portBox = new QSpinBox(this);
    ui_portBox->setMinimum(0);
    ui_portBox->setMaximum(65535);
    ui_portBox->setValue(443);
    formLayout->addWidget(ui_portBox, 2, 1);

    QLabel *updateFreqLabel = new QLabel(tr("Sync every"), this);
    formLayout->addWidget(updateFreqLabel, 3, 0);

    ui_updateFreqSpinBox = new QSpinBox(this);
    ui_updateFreqSpinBox->setMinimum(15);
    ui_updateFreqSpinBox->setMaximum(600);
    ui_updateFreqSpinBox->setValue(60);
    ui_updateFreqSpinBox->setSuffix(" seconds");
    formLayout->addWidget(ui_updateFreqSpinBox, 3, 1);

    QLabel *timeOutLabel = new QLabel(tr("Server timeout"), this);
    formLayout->addWidget(timeOutLabel, 4, 0);

    ui_timeoutSpinBox = new QSpinBox(this);
    ui_timeoutSpinBox->setMinimum(1);
    ui_timeoutSpinBox->setMaximum(10);
    ui_timeoutSpinBox->setValue(3);
    ui_timeoutSpinBox->setSuffix(" seconds");
    formLayout->addWidget(ui_timeoutSpinBox, 4, 1);

    ui_orderServerButton = new QPushButton(
                tr("If you don't have access to a server yet,\n"
                   "you can order one on ramses.rxlab.io"),
                this
                );
    formLayout->addWidget(ui_orderServerButton,5,1);
}

void ServerEditWidget::connectEvents()
{
    connect(ui_orderServerButton, &QPushButton::clicked, this, &ServerEditWidget::orderServer);
    connect(ui_sslCheckBox, &QCheckBox::clicked, this, &ServerEditWidget::updatePort);

    connect(ui_serverAddressEdit, &QLineEdit::textEdited,
            this, &ServerEditWidget::addressEdited);
}
