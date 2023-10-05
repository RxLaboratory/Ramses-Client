#include "objectmanagerwidget.h"

ObjectManagerWidget::ObjectManagerWidget(QString title, QIcon icon, QWidget *parent) :
    QWidget(parent)
{
    setupUi(title,icon);
    connectEvents();
    ui_listWidget->setTitle(title);
    clear();
}

ObjectManagerWidget::ObjectManagerWidget(RamAbstractObjectModel *objectModel, QString title, QIcon icon, QWidget *parent) :
    QWidget(parent)
{
    setupUi(title,icon);
    connectEvents();
    ui_listWidget->setTitle(title);
    setObjectModel(objectModel);
}

void ObjectManagerWidget::setObjectModel(QAbstractItemModel *objectModel)
{
    ui_listWidget->setObjectModel( objectModel );
    if (!objectModel) return;
    this->setEnabled(true);
}

void ObjectManagerWidget::clear()
{
    this->setEnabled(false);
    ui_listWidget->clear();
}

void ObjectManagerWidget::setupUi(QString title, QIcon icon)
{
    QHBoxLayout *lay = new QHBoxLayout(this);
    lay->setContentsMargins(3,0,0,0);

    ui_listWidget = new ObjectListWidget( false, RamUser::Admin, this);
    //ui_listWidget->setMaximumWidth(500);
    lay->addWidget(ui_listWidget);

    //lay->addStretch();

    // Menu
    DuMenu *itemMenu = new DuMenu();

    ui_createAction = new QAction("Create new", this);
    itemMenu->addAction(ui_createAction);

    ui_itemButton = new QToolButton();
    ui_itemButton->setIcon(icon);
    ui_itemButton->setText(" " + title);
    ui_itemButton->setMenu(itemMenu);
    ui_itemButton->setIconSize(QSize(16,16));
    ui_itemButton->setObjectName("menuButton");
    ui_itemButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    ui_itemButton->setPopupMode(QToolButton::InstantPopup);

    // Shortcuts
    ui_createShortcut = new QShortcut(QKeySequence("Shift+A"),this);
    ui_removeShortcut = new QShortcut(QKeySequence("Shift+X"),this);
}

void ObjectManagerWidget::connectEvents()
{
    connect( ui_listWidget, SIGNAL(add()), this, SLOT(createObject()) );
    connect( ui_createAction, SIGNAL(triggered()), this, SLOT(createEditObject()) );
    connect( ui_createShortcut, SIGNAL(activated()), this, SLOT(createObject()));
    connect( ui_removeShortcut, SIGNAL(activated()), ui_listWidget, SLOT(removeSelectedObjects()));
}

QString ObjectManagerWidget::currentFilterUuid() const
{
    return ui_listWidget->currentFilterUuid();
}

RamObject *ObjectManagerWidget::currentFilter() const
{
    return ui_listWidget->currentFilter();
}

QToolButton *ObjectManagerWidget::menuButton()
{
    return ui_itemButton;
}

void ObjectManagerWidget::createEditObject()
{
    RamObject *o = createObject();
    if (o) o->edit();
}
