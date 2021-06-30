#include "titlebar.h"

TitleBar::TitleBar(QString title, bool mini, QWidget *parent) :
    QToolBar(parent)
{
    setupUi(mini);

    this->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);

    titleLabel->setText(title);
    reinitButton->setToolTip("Reinitialize " + title);
    closeButton->setToolTip("Close " + title);

    reinitButton->setObjectName("windowButton");
    closeButton->setObjectName("windowButton");

    connect(reinitButton, &QToolButton::clicked, this, &TitleBar::reinitRequested);
    connect(closeButton, &QToolButton::clicked, this, &TitleBar::closeRequested);

    this->setObjectName("titleBar");
}

void TitleBar::setTitle(QString title)
{
    titleLabel->setText(title);
    reinitButton->setToolTip("Reinitialize " + title);
    reinitButton->setStatusTip("Reinitialize " + title);
    closeButton->setToolTip("Close " + title);
    closeButton->setStatusTip("Close " + title);
}

void TitleBar::showReinitButton(bool show)
{
    reinitAction->setVisible(show);
}

void TitleBar::insertRight(QWidget *w)
{
    this->insertWidget(rightAction, w);
}

void TitleBar::insertRight(QAction *a)
{
    this->insertAction(rightAction, a);
}

void TitleBar::insertLeft(QWidget *w)
{
    this->insertWidget(leftAction, w);
}

void TitleBar::insertLeft(QAction *a)
{
    this->insertAction(leftAction, a);
}

void TitleBar::setupUi(bool mini)
{
    this->setObjectName(QStringLiteral("TitleBar"));

    titleLabel = new QLabel(this);

    reinitButton = new QToolButton(this);
    reinitButton->setIcon(QIcon(":/icons/reinit"));

    closeButton = new QToolButton(this);
    closeButton->setIcon(QIcon(":/icons/close"));


    if (mini)
    {
        this->addWidget(titleLabel);
        leftAction = rightAction = reinitAction = this->addWidget(reinitButton);
        this->addWidget(closeButton);
    }
    else
    {
        this->addWidget(titleLabel);
        spacer = new ToolBarSpacer(this);
        leftAction = this->addWidget(spacer);
        rightAction = this->addWidget(reinitButton);
        reinitAction = rightAction;
        this->addWidget(closeButton);
    }
}
