#include "templateassetgroupeditwidget.h"

TemplateAssetGroupEditWidget::TemplateAssetGroupEditWidget(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);

    connect(updateButton, SIGNAL(clicked()), this, SLOT(update()));
    connect(revertButton, SIGNAL(clicked()), this, SLOT(revert()));
    connect(shortNameEdit, &QLineEdit::textChanged, this, &TemplateAssetGroupEditWidget::checkInput);
    connect(DBInterface::instance(),&DBInterface::log, this, &TemplateAssetGroupEditWidget::dbiLog);

    this->setEnabled(false);
}

RamAssetGroup *TemplateAssetGroupEditWidget::assetGroup() const
{
    return _assetGroup;
}

void TemplateAssetGroupEditWidget::setAssetGroup(RamAssetGroup *assetGroup)
{
    disconnect(_currentAssetGroupConnection);

    _assetGroup = assetGroup;
    nameEdit->setText("");
    shortNameEdit->setText("");

    this->setEnabled(false);

    if (!assetGroup) return;

    nameEdit->setText(assetGroup->name());
    shortNameEdit->setText(assetGroup->shortName());

    this->setEnabled(Ramses::instance()->isAdmin());

    _currentAssetGroupConnection = connect(assetGroup, &RamAssetGroup::destroyed, this, &TemplateAssetGroupEditWidget::assetGroupDestroyed);
}

void TemplateAssetGroupEditWidget::update()
{
    if (!_assetGroup) return;

    this->setEnabled(false);

    //check if everything is alright
    if (!checkInput())
    {
        this->setEnabled(true);
        return;
    }

    _assetGroup->setName(nameEdit->text());
    _assetGroup->setShortName(shortNameEdit->text());

    _assetGroup->update();

    this->setEnabled(true);

}

void TemplateAssetGroupEditWidget::revert()
{
    setAssetGroup(_assetGroup);
}

bool TemplateAssetGroupEditWidget::checkInput()
{
    if (!_assetGroup) return false;

    if (shortNameEdit->text() == "")
    {
        statusLabel->setText("Short name cannot be empty!");
        updateButton->setEnabled(false);
        return false;
    }

    statusLabel->setText("");
    updateButton->setEnabled(true);
    return true;
}

void TemplateAssetGroupEditWidget::assetGroupDestroyed(QObject */*o*/)
{
    setAssetGroup(nullptr);
}

void TemplateAssetGroupEditWidget::dbiLog(QString m, LogUtils::LogType t)
{
    if (t != LogUtils::Remote && t != LogUtils::Debug) statusLabel->setText(m);
}
