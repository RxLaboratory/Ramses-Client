#include "homepage.h"

#include "duapp/duui.h"
#include "progressmanager.h"

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
    auto pm = ProgressManager::i();
    ui_waitIcon->show();
    ui_progressTextLabel->setText(pm->text());
    ui_progressTitleLabel->setText(pm->title());
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
    textLayout->addWidget(ui_progressTitleLabel);

    ui_progressTextLabel = new QLabel(this);
    textLayout->addWidget(ui_progressTextLabel);
}

void HomePage::connectEvents()
{
    auto pm = ProgressManager::i();
    connect(pm, &ProgressManager::textChanged,
            ui_progressTextLabel, &QLabel::setText);
    connect(pm, &ProgressManager::titleChanged,
            ui_progressTitleLabel, &QLabel::setText);
    connect(pm, &ProgressManager::finished,
            this, &HomePage::setWaiting);
    connect(pm, &ProgressManager::started,
            this, &HomePage::setWorking);

    connect(pm, SIGNAL(textChanged(QString)),
            this, SLOT(repaint()));
    connect(pm, SIGNAL(titleChanged(QString)),
            this, SLOT(repaint()));
}
