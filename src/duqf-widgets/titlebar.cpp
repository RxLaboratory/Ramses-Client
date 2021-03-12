#include "titlebar.h"

TitleBar::TitleBar(QString title, QWidget *parent) :
    QToolBar(parent)
{
    setupUi();

    titleLabel->setText(title);
    reinitButton->setToolTip("Reinitialize " + title);
    closeButton->setToolTip("Close " + title);

    reinitButton->setObjectName("windowButton");
    closeButton->setObjectName("windowButton");

    connect(reinitButton, &QToolButton::clicked, this, &TitleBar::reinitRequested);
    connect(closeButton, &QToolButton::clicked, this, &TitleBar::closeRequested);

    this->setObjectName("titleBar");
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

void TitleBar::setupUi()
{
    this->setObjectName(QStringLiteral("TitleBar"));
    this->resize(378, 29);


    titleLabel = new QLabel(this);
    this->addWidget(titleLabel);

    leftAction = this->addWidget(new ToolBarSpacer(this));

    reinitButton = new QToolButton(this);
    reinitButton->setIcon(QIcon(":/icons/reinit"));
    rightAction = this->addWidget(reinitButton);
    reinitAction = rightAction;

    closeButton = new QToolButton(this);
    closeButton->setIcon(QIcon(":/icons/close"));
    this->addWidget(closeButton);
}
