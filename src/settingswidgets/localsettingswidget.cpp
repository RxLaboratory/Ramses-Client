#include "localsettingswidget.h"

LocalSettingsWidget::LocalSettingsWidget(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);

    folderSelector = new DuQFFolderSelectorWidget(DuQFFolderSelectorWidget::Folder, this);
    mainFolderLayout->addWidget(folderSelector);

    folderSelector->setPath( Ramses::instance()->pathFromRamses() );

    QDateTime d = QDateTime::fromString("2021-04-26 10:53:31", "yyyy-MM-dd hh:mm:ss");
    QString f = "yyyy-MM-dd hh:mm:ss";
    dateFormatBox->addItem(d.toString(f), f);
    f = "MM.dd.yyyy - h:mm:ss ap";
    dateFormatBox->addItem(d.toString(f), f);
    f = "dd/MM/yyyy - hh:mm:ss";
    dateFormatBox->addItem(d.toString(f), f);
    f = "ddd MMMM d yyyy 'at' h:mm:ss ap";
    dateFormatBox->addItem(d.toString(f), f);
    f = "ddd d MMMM yyyy 'at' hh:mm:ss";
    dateFormatBox->addItem(d.toString(f), f);

    QString currentF = m_settings.value("dateFormat", "yyyy-MM-dd hh:mm:ss").toString();
    for (int i =0; i < dateFormatBox->count(); i++)
        if (dateFormatBox->itemData(i).toString() == currentF ) dateFormatBox->setCurrentIndex(i);

    connect(folderSelector, &DuQFFolderSelectorWidget::pathChanged, this, &LocalSettingsWidget::setRamsesPath);
    connect(dateFormatBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setDateFormat()));
}

void LocalSettingsWidget::setRamsesPath(QString p)
{
    Ramses::instance()->setRamsesPath(p);
}

void LocalSettingsWidget::setDateFormat()
{
    RamUser *user = Ramses::instance()->currentUser();
    if (!user ) return;
    QSettings *userSettings = user->settings();
    userSettings->setValue("ramses/dateFormat", dateFormatBox->currentData().toString());
}
