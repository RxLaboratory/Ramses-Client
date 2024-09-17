#include "homepage.h"

#include "duapp/duui.h"

HomePage::HomePage(QWidget *parent)
    : QWidget{parent}
{
    setupUi();
    connectEvents();
}

void HomePage::changeState(StateManager::State s)
{
    switch (s) {
    case StateManager::Unknown:
    case StateManager::Idle:
        ui_waitIcon->hide();
        ui_progressTitleLabel->setText(tr("Ready."));
        ui_progressTextLabel->setText(tr("I hope you're feeling well today!"));
        break;
    case StateManager::Opening:
    case StateManager::WritingDataBase:
    case StateManager::Connecting:
    case StateManager::LoadingDataBase:
    case StateManager::Closing:
    case StateManager::Syncing:
        ui_progressTitleLabel->setText(StateManager::i()->title());
        ui_progressTextLabel->setText(StateManager::i()->text());
        ui_waitIcon->show();
        break;
    }
}

void HomePage::setupUi()
{
    auto layout = DuUI::createBoxLayout(Qt::Horizontal, false);
    layout->setSpacing(24);
    DuUI::centerLayout(layout, this, 1, 150);

    ui_waitIcon = new DuIconWidget(this);
    ui_waitIcon->setSize(QSize(48,48));
    ui_waitIcon->setSVGIcon(":/icons/wait");
    ui_waitIcon->rotate();
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
            this, &HomePage::changeState);
}
