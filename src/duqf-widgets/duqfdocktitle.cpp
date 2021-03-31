#include "duqfdocktitle.h"

DuQFDockTitle::DuQFDockTitle(QString title, QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *vlayout = new QVBoxLayout(this);
    vlayout->setContentsMargins(0,3,0,3);
    vlayout->setSpacing(0);

    // include in a frame for the BG
    QFrame *mainFrame = new QFrame(this);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(3,0,3,0);
    layout->setSpacing(3);

    QToolButton *pinButton = new QToolButton(this);
    pinButton->setIcon(QIcon(":/icons/pin"));
    pinButton->setCheckable(true);
    pinButton->setObjectName("windowButton");
    layout->addWidget(pinButton);

    layout->addStretch();

    _titleLabel = new QLabel(title, this);
    layout->addWidget(_titleLabel);

    layout->addStretch();

    QToolButton *closeButton = new QToolButton(this);
    closeButton->setIcon(QIcon(":/icons/close"));
    closeButton->setObjectName("windowButton");

    layout->addWidget(closeButton);

    mainFrame->setLayout(layout);
    vlayout->addWidget(mainFrame);

    this->setLayout(vlayout);

    connect(closeButton, &QToolButton::clicked, this, &DuQFDockTitle::closeDockWidget);
    connect(pinButton, &QToolButton::clicked, this, &DuQFDockTitle::pinDockWidget);
}

void DuQFDockTitle::setTitle(QString title)
{
    _titleLabel->setText(title);
}

void DuQFDockTitle::closeDockWidget()
{
    emit pinDockWidget(false);
    parentWidget()->hide();
}
