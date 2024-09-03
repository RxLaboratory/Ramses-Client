#include "landingpage.h"

#include <QLabel>
#include <QFileInfo>
#include <QtDebug>
#include <QFileDialog>

#include "duapp/app-version.h"
#include "projectmanager.h"
#include "duapp/duui.h"
#include "duwidgets/duicon.h"

LandingPage::LandingPage(QWidget *parent)
    : QWidget{parent}
{
    setupUi();
    connectEvents();
    updateRecentList();
}

void LandingPage::updateRecentList()
{
    ui_recentBox->clear();
    const QStringList projects = ProjectManager::i()->recentProjects();
    for(const auto &project: projects) {
        QFileInfo f(project);
        ui_recentBox->addItem(f.completeBaseName(), project);
    }
}

void LandingPage::setupUi()
{
    auto mainLayout = DuUI::addBoxLayout(Qt::Horizontal, this);
    mainLayout->addStretch();
    auto centerLayout = DuUI::addBoxLayout(Qt::Vertical, mainLayout);
    mainLayout->addStretch();

    centerLayout->addStretch(2);

    QLabel *logoLabel = new QLabel(this);
    logoLabel->setPixmap(QPixmap(":/icons/logo_large"));
    logoLabel->setMinimumSize(QSize(256, 256));
    logoLabel->setMaximumSize(QSize(256, 256));
    logoLabel->setScaledContents(true);
    logoLabel->setAlignment(Qt::AlignCenter);
    centerLayout->addWidget(logoLabel);
    centerLayout->setAlignment(logoLabel, Qt::AlignCenter);

    centerLayout->addStretch(1);

    auto buttonLayout = DuUI::addGridLayout(centerLayout);

    ui_createLocalProjectButton = new QPushButton(tr("Create\nproject..."), this);
    ui_createLocalProjectButton->setIcon(DuIcon(":/icons/ramses-file"));
    ui_createLocalProjectButton->setIconSize(QSize(32,32));
    buttonLayout->addWidget(ui_createLocalProjectButton, 0, 0);

    ui_createTeamProjectButton = new QPushButton(tr("Create\nteam project..."), this);
    ui_createTeamProjectButton->setIcon(DuIcon(":/icons/users"));
    ui_createTeamProjectButton->setIconSize(QSize(32,32));
    buttonLayout->addWidget(ui_createTeamProjectButton, 0, 1);

    ui_openProjectButton = new QPushButton(tr("Open\nproject..."), this);
    ui_openProjectButton->setIcon(DuIcon(":/icons/open"));
    ui_openProjectButton->setIconSize(QSize(32,32));
    buttonLayout->addWidget(ui_openProjectButton, 1, 0);

    ui_joinTeamProjectButton = new QPushButton(tr("Join\nteam project..."), this);
    ui_joinTeamProjectButton->setIcon(DuIcon(":/icons/login"));
    ui_joinTeamProjectButton->setIconSize(QSize(32,32));
    buttonLayout->addWidget(ui_joinTeamProjectButton, 1, 1);

    auto recentLayout = DuUI::addBoxLayout(Qt::Horizontal, centerLayout);

    recentLayout->addWidget(
        new QLabel( tr("Open recent:"), this)
        );

    ui_recentBox = new DuComboBox(this);
    recentLayout->addWidget(ui_recentBox);

    ui_openRecentProjectButton = new QToolButton(this);
    ui_openRecentProjectButton->setIcon(DuIcon(":/icons/check"));
    ui_openRecentProjectButton->setEnabled(ui_recentBox->currentIndex() >= 0);
    recentLayout->addWidget(ui_openRecentProjectButton);

    recentLayout->setStretch(0, 0);
    recentLayout->setStretch(1, 1);
    recentLayout->setStretch(2, 0);

    centerLayout->addStretch(2);
}

void LandingPage::connectEvents()
{
    connect(ui_recentBox, QOverload<int>::of( &DuComboBox::currentIndexChanged ),
            this, [this] (int index) {
        ui_openRecentProjectButton->setEnabled(index >= 0);
    });

    connect(ui_openRecentProjectButton, &QToolButton::clicked,
            this, [this] () {
        ProjectManager::i()->setProject(ui_recentBox->currentData().toString());
    });

    connect(ui_openProjectButton, &QPushButton::clicked,
            this, [this] () {
        QString p = QFileDialog::getOpenFileName(
            this,
            STR_INTERNALNAME,
            "",
            "Ramses Project (*.ramses);;SQLite (*.sqlite);;All Files (*.*)");
        if (p == "") return;
        ProjectManager::i()->setProject(p);
    });
}
