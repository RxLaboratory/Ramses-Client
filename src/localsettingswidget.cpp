#include "localsettingswidget.h"

LocalSettingsWidget::LocalSettingsWidget(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);

    folderSelector = new DuQFFolderSelectorWidget(this);
    mainFolderLayout->addWidget(folderSelector);

    folderSelector->setPath(_settings.value("ramsesPath", QDir::homePath() + "/Ramses").toString());

    connect(folderSelector, &DuQFFolderSelectorWidget::pathChanged, this, &LocalSettingsWidget::setRamsesPath);
}

void LocalSettingsWidget::setRamsesPath(QString p)
{
    _settings.setValue("ramsesPath", p);
    Ramses::instance()->setMainPath(p);
}
