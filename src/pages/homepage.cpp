#include "homepage.h"

#include "duapp/duui.h"
#include "statemanager.h"

HomePage::HomePage(QWidget *parent)
    : QWidget{parent}
{
    setupUi();
    connectEvents();
    setWaiting();
}

void HomePage::setWaiting()
{
    ui_waitIcon->hide();
    ui_progressTextLabel->setText(tr("I hope you're feeling well today!")); // TODO Add random quote
    ui_progressTitleLabel->setText(tr("Ready."));
}

void HomePage::setWorking()
{
    auto sm = StateManager::i();
    ui_waitIcon->show();
    ui_progressTextLabel->setText(sm->text());
    ui_progressTitleLabel->setText(sm->title());
}

void HomePage::setupUi()
{
    auto layout = DuUI::createBoxLayout(Qt::Horizontal, false);
    layout->setSpacing(24);
    DuUI::centerLayout(layout, this, 1, 150);

    ui_waitIcon = new DuWaitIconWidget(1, 24, this);
    ui_waitIcon->setSize(QSize(48,48));
    layout->addWidget(ui_waitIcon);

    auto textLayout = DuUI::addBoxLayout(Qt::Vertical, layout);

    ui_progressTitleLabel = new QLabel(this);
    DuUI::addCustomCSS(ui_progressTitleLabel, "QLabel { font-weight: 900;  color: @focus-pull; }");
    textLayout->addWidget(ui_progressTitleLabel);

    ui_progressTextLabel = new QLabel(this);
    DuUI::addCustomCSS(ui_progressTextLabel, "QLabel { color: @foreground-push; }");
    textLayout->addWidget(ui_progressTextLabel);
}

void HomePage::connectEvents()
{
    auto sm = StateManager::i();
    connect(sm, &StateManager::textChanged,
            ui_progressTextLabel, &QLabel::setText);
    connect(sm, &StateManager::titleChanged,
            ui_progressTitleLabel, &QLabel::setText);
    connect(sm, &StateManager::stateChanged,
            this,[this] (StateManager::State s) {
        if (s == StateManager::Idle) setWaiting();
        else setWorking();
    });
}
