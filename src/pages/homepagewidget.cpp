#include "homepagewidget.h"

#include "duapp/duui.h"
#include "duapp/dusettings.h"

HomePageWidget::HomePageWidget(QWidget *parent)
    : QWidget{parent}
{
    setupUi();
    connectEvents();
}

void HomePageWidget::changeState(StateManager::State s)
{
    switch (s) {
    case StateManager::Unknown:
    case StateManager::Idle:
        ui_waitIcon->hide();
        ui_progressTitleLabel->setText(tr("Ready."));
        ui_progressTextLabel->setText(tr("I hope you're feeling well today!"));
        break;
    case StateManager::Opening:
    case StateManager::Connecting:
    case StateManager::Closing:
    case StateManager::Syncing:
        ui_progressTitleLabel->setText(StateManager::i()->title());
        ui_progressTextLabel->setText(StateManager::i()->text());
        ui_waitIcon->show();
        break;
    }
}

void HomePageWidget::setupUi()
{
    auto layout = DuUI::createBoxLayout(Qt::Horizontal, false);
    layout->setSpacing(24);
    DuUI::centerLayout(layout, this, 1, 150);

    ui_waitIcon = new DuIconWidget(this);
    ui_waitIcon->setSize(QSize(48,48));
    ui_waitIcon->setSVGIcon(":/icons/wait");
    ui_waitIcon->rotate();
    ui_waitIcon->setColor(
        DuSettings::i()->get(DuSettings::UI_FocusColor).value<QColor>()
        );
    layout->addWidget(ui_waitIcon);

    auto textLayout = DuUI::addBoxLayout(Qt::Vertical, layout);

    ui_progressTitleLabel = new QLabel(this);
    DuUI::addCustomCSS(ui_progressTitleLabel, "QLabel { font-weight: 900;  color: @focus-pull; }");
    textLayout->addWidget(ui_progressTitleLabel);

    ui_progressTextLabel = new QLabel(this);
    DuUI::addCustomCSS(ui_progressTextLabel, "QLabel { color: @foreground-push; }");
    ui_progressTextLabel->setWordWrap(true);
    ui_progressTextLabel->setMaximumWidth(400);
    textLayout->addWidget(ui_progressTextLabel);
}

void HomePageWidget::connectEvents()
{
    auto sm = StateManager::i();
    connect(sm, &StateManager::textChanged,
            ui_progressTextLabel, &QLabel::setText);
    connect(sm, &StateManager::titleChanged,
            ui_progressTitleLabel, &QLabel::setText);
    connect(sm, &StateManager::stateChanged,
            this, &HomePageWidget::changeState);
}
