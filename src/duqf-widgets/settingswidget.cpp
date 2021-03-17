#include "settingswidget.h"

SettingsWidget::SettingsWidget(QString title, QWidget *parent) :
    QWidget(parent)
{
    setupUi(title);

    connect(titleBar, &TitleBar::reinitRequested, this, &SettingsWidget::reinitRequested);
    connect(titleBar, &TitleBar::closeRequested, this, &SettingsWidget::closeRequested);
    connect(mainList, SIGNAL(currentRowChanged(int)), this, SLOT(mainList_currentRowChanged(int)));
}

void SettingsWidget::setupUi(QString title)
{
    this->setObjectName(QStringLiteral("SettingsWidget"));
    QVBoxLayout *verticalLayout = new QVBoxLayout(this);
    verticalLayout->setContentsMargins(0, 0, 0, 0);
    verticalLayout->setSpacing(0);

    titleBar = new TitleBar(title,this);
    verticalLayout->addWidget(titleBar);

    QWidget *mainWidget = new QWidget(this);
    verticalLayout->addWidget(mainWidget);

    QHBoxLayout *horizontalLayout = new QHBoxLayout(mainWidget);
    horizontalLayout->setSpacing(0);
    horizontalLayout->setContentsMargins(0, 0, 0, 0);

    QSplitter *splitter = new QSplitter(this);
    splitter->setHandleWidth(9);

    mainList = new QListWidget(this);
    mainList->setFrameShape(QFrame::NoFrame);
    mainList->setLineWidth(0);
    mainList->setResizeMode(QListView::Adjust);
    mainList->setLayoutMode(QListView::SinglePass);

    splitter->addWidget(mainList);

    mainStackWidget = new QStackedWidget(this);
    mainStackWidget->setObjectName(QStringLiteral("mainStackWidget"));
    QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Preferred);
    mainStackWidget->setSizePolicy(sizePolicy1);

    splitter->addWidget(mainStackWidget);

    horizontalLayout->addWidget(splitter);

    mainStackWidget->setCurrentIndex(-1);
}

void SettingsWidget::addPage(QWidget *ui, QString title, QIcon icon)
{
    mainStackWidget->addWidget(ui);
    QListWidgetItem *tab = new QListWidgetItem(icon,title);
    mainList->addItem(tab);
}

void SettingsWidget::setCurrentIndex(int index)
{
    mainStackWidget->setCurrentIndex(index);
}

void SettingsWidget::showReinitButton(bool show)
{
    titleBar->showReinitButton(show);
}

void SettingsWidget::mainList_currentRowChanged(int currentRow)
{
    mainStackWidget->setCurrentIndex(currentRow);
}
