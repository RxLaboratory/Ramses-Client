#include "settingswidget.h"

SettingsWidget::SettingsWidget(QWidget *parent) :
    QWidget(parent)
{
    setupUi();

    connect(m_mainList, SIGNAL(currentRowChanged(int)), this, SLOT(mainList_currentRowChanged(int)));
}

void SettingsWidget::setupUi()
{
    this->setObjectName(QStringLiteral("SettingsWidget"));
    QVBoxLayout *verticalLayout = new QVBoxLayout(this);
    verticalLayout->setContentsMargins(0, 0, 0, 0);
    verticalLayout->setSpacing(0);

    QWidget *mainWidget = new QWidget(this);
    verticalLayout->addWidget(mainWidget);

    QHBoxLayout *horizontalLayout = new QHBoxLayout(mainWidget);
    horizontalLayout->setSpacing(0);
    horizontalLayout->setContentsMargins(0, 0, 0, 0);

    m_splitter = new QSplitter(this);
    m_splitter->setHandleWidth(6);

    m_mainList = new DuListWidget(this);
    m_mainList->setFrameShape(QFrame::NoFrame);
    m_mainList->setLineWidth(0);
    m_mainList->setResizeMode(QListView::Adjust);
    m_mainList->setLayoutMode(QListView::SinglePass);

    m_splitter->addWidget(m_mainList);

    m_mainStackWidget = new QStackedWidget(this);
    m_mainStackWidget->setObjectName(QStringLiteral("mainStackWidget"));
    QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_mainStackWidget->setSizePolicy(sizePolicy1);

    m_splitter->addWidget(m_mainStackWidget);

    horizontalLayout->addWidget(m_splitter);

    m_mainStackWidget->setCurrentIndex(-1);
}

void SettingsWidget::addPage(QWidget *ui, QString title, QIcon icon)
{
    m_mainStackWidget->addWidget(ui);
    QListWidgetItem *tab = new QListWidgetItem(icon,title);
    m_mainList->addItem(tab);
}

void SettingsWidget::setCurrentIndex(int index)
{
    m_mainStackWidget->setCurrentIndex(index);
}

void SettingsWidget::showEvent(QShowEvent *event)
{
    m_splitter->setSizes(QList<int>() << 200 << 1000);
    QWidget::showEvent(event);
}

void SettingsWidget::hideEvent(QHideEvent *event)
{
    QWidget::hideEvent(event);
}

void SettingsWidget::mainList_currentRowChanged(int currentRow)
{
    this->setCurrentIndex(currentRow);
}
