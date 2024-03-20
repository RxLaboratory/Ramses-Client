#include "dudocktitlewidget.h"
#include "duqf-widgets/duicon.h"
#include "duqf-app/duui.h"

DuDockTitleWidget::DuDockTitleWidget(QString title, QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *vlayout = new QVBoxLayout(this);
    vlayout->setContentsMargins(0,0,0,0);
    vlayout->setSpacing(0);

    // include in a frame for the BG
    QFrame *mainFrame = new QFrame(this);

    _layout = new QHBoxLayout(mainFrame);
    _layout->setContentsMargins(16,0,6,0);
    _layout->setSpacing(DuUI::adjustToDpi(3));

    _iconLabel = new QLabel(this);
    _iconLabel->setScaledContents(true);
    _iconLabel->setFixedSize(QSize(16,16));
    _layout->addWidget(_iconLabel);

    _titleLabel = new DuLabel(title, Qt::ElideRight, this);
    _layout->addWidget(_titleLabel);

    _menuButton = new QToolButton(this);
    _menuButton->setIcon(DuIcon(":/icons/list"));
    _menuButton->setFixedSize(
        DuUI::adjustToDpi(QSize(22,22))
        );
    _menuButton->setToolTip(tr("Presets and options"));
    _menuButton->setPopupMode(QToolButton::InstantPopup);
    _menuButton->setVisible(false);
    _layout->addWidget(_menuButton);

    QToolButton *closeButton = new QToolButton(this);
    closeButton->setIcon(DuIcon(":/icons/close"));
    closeButton->setFixedSize(
        DuUI::adjustToDpi(QSize(24,24))
        );
    closeButton->setObjectName("windowButton");

    _layout->addWidget(closeButton);

    _layout->setStretch(0, 0);
    _layout->setStretch(0, 1);

    mainFrame->setLayout(_layout);
    vlayout->addWidget(mainFrame);

    this->setLayout(vlayout);

    connect(closeButton, &QToolButton::clicked, this, &DuDockTitleWidget::closeDockWidget);

    // Set a minimum size
    this->setMinimumSize(
        DuUI::adjustToDpi(QSize(28,28))
        );
    // But make the frame shrinkable as much as possible
    mainFrame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mainFrame->setMinimumSize(1,1);
}

void DuDockTitleWidget::setTitle(QString title)
{
    _titleLabel->setText(title);
}

void DuDockTitleWidget::setIcon(QString icon)
{
    _iconLabel->setPixmap(DuIcon(icon).pixmap(QSize(16,16)));
}

void DuDockTitleWidget::setMenu(QMenu *menu)
{
    if (_menu) disconnect(_menu, nullptr, this, nullptr);

    _menu = menu;
    _menuButton->setMenu(menu);
    _menuButton->setVisible(!menu->actions().isEmpty());
}

void DuDockTitleWidget::addActionButton(QAction *action)
{
    QToolButton *b = new QToolButton(this);
    b->setFixedSize(
        DuUI::adjustToDpi(QSize(22,22))
        );
    b->setDefaultAction(action);
    _layout->insertWidget(2, b);
    _actionButtons << b;
}

void DuDockTitleWidget::setActionButtons(const QVector<QAction *> &actions)
{
    qDeleteAll(_actionButtons);
    _actionButtons.clear();
    for (auto a: actions) addActionButton(a);
}

void DuDockTitleWidget::closeDockWidget()
{
    parentWidget()->hide();
}
