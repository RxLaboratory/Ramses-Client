#include "databaseeditwidget.h"

#include "ramdatainterface/localdatainterface.h"
#include "mainwindow.h"

DatabaseEditWidget::DatabaseEditWidget(QWidget *parent) :
    QScrollArea(parent)
{
    setupUi();
    connectEvents();
}

const QString &DatabaseEditWidget::dbFile() const
{
    return m_dbFile;
}

void DatabaseEditWidget::setDbFile(const QString &newDbFile)
{
    m_dbFile = newDbFile;

    // Get settings
    ServerConfig s = LocalDataInterface::getServerSettings(m_dbFile);

    ui_serverEdit->setAddress(s.address);
    ui_serverEdit->setSsl(s.useSsl);
    ui_serverEdit->setTimeout(s.timeout);
    ui_serverEdit->setUpdateFreq(s.updateDelay);

    // Check if the address is valid
    bool sync = ui_serverEdit->address() != "";
    ui_syncBox->setChecked(sync);
    setSync(sync);
}

void DatabaseEditWidget::setSync(bool s)
{
    if (s) ui_syncBox->setText(tr("Online (Sync)"));
    else ui_syncBox->setText(tr("Offline (No sync)"));

    ui_serverEdit->setEnabled(s);
}

void DatabaseEditWidget::apply()
{
    ServerConfig s;

    if (ui_syncBox->isChecked())
    {
        s.address = ui_serverEdit->address();
        if (s.address == "") return;
        s.useSsl = ui_serverEdit->ssl();
        s.updateDelay = ui_serverEdit->updateFreq();
        s.timeout = ui_serverEdit->timeout();
    }

    LocalDataInterface::setServerSettings(m_dbFile, s);

    emit applied();
}

void DatabaseEditWidget::reset()
{
    setDbFile(m_dbFile);
}

void DatabaseEditWidget::setupUi()
{
    QWidget *dummy = new QWidget(this);
    dummy->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->setWidget(dummy);
    this->setWidgetResizable(true);
    this->setFrameStyle(QFrame::NoFrame);

    QVBoxLayout *mainLayout = new QVBoxLayout(dummy);
    mainLayout->setSpacing(3);
    mainLayout->setContentsMargins(3, 3, 3, 3);

    ui_syncBox = new QCheckBox(tr("Offline (no sync)"), dummy);
    mainLayout->addWidget(ui_syncBox);

    ui_serverEdit = new ServerEditWidget();
    ui_serverEdit->setEnabled(false);
    mainLayout->addWidget(ui_serverEdit);

    QHBoxLayout *validButtons = new QHBoxLayout();
    validButtons->setSpacing(3);
    validButtons->setContentsMargins(0,0,0,0);
    mainLayout->addLayout(validButtons);

    ui_resetButton = new QPushButton(tr("Reset"));
    ui_resetButton->setIcon(QIcon(":/icons/reinit"));
    validButtons->addWidget(ui_resetButton);

    ui_applyButton = new QPushButton(tr("Apply changes"));
    ui_applyButton->setIcon(QIcon(":/icons/apply"));
    validButtons->addWidget(ui_applyButton);

    mainLayout->addStretch();
}

void DatabaseEditWidget::connectEvents()
{
    connect(ui_syncBox, &QCheckBox::clicked, this, &DatabaseEditWidget::setSync);
    connect(ui_resetButton, &QPushButton::clicked, this, &DatabaseEditWidget::reset);
    connect(ui_applyButton, &QPushButton::clicked, this, &DatabaseEditWidget::apply);
}
