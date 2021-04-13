#include "ramobjectwidget.h"

RamObjectWidget::RamObjectWidget(RamObject *obj, QWidget *parent) : QWidget(parent)
{
    setupUi();

    setObject(obj);
    userChanged();

    // Build a dock widget to edit details
    _dockEditWidget = new ObjectDockWidget(obj);
    _dockEditWidget->hide();

    connect(obj, &RamObject::changed, this, &RamObjectWidget::setObject);
    connect(obj, &RamObject::removed, this, &RamObjectWidget::objectRemoved);
    connect(Ramses::instance(), &Ramses::loggedIn, this, &RamObjectWidget::userChanged);
    connect(editButton, SIGNAL(clicked()), this, SLOT(edit()));
}

RamObjectWidget::~RamObjectWidget()
{
    _dockEditWidget->deleteLater();
}

RamObject *RamObjectWidget::ramObject() const
{
    return _object;
}

void RamObjectWidget::setObject(RamObject *o)
{
    _object = o;
    if (o) setTitle(o->name());
    else
    {
        setTitle("");
        setEnabled(false);
    }
}

void RamObjectWidget::setUserEditRole(RamUser::UserRole role)
{
    _editRole = role;
}

void RamObjectWidget::setTitle(QString t)
{
    title->setText(t);
}

ObjectDockWidget *RamObjectWidget::dockEditWidget() const
{
    return _dockEditWidget;
}

void RamObjectWidget::setEditWidget(ObjectEditWidget *w)
{
    w->setParent(_dockEditWidget);
    _dockEditWidget->setWidget(w);
    _hasEditWidget = true;
}

void RamObjectWidget::objectRemoved()
{
    deleteLater();
}

void RamObjectWidget::userChanged()
{
    editButton->hide();
    RamUser *u = Ramses::instance()->currentUser();
    if (!u) return;
    editButton->setVisible( u->role() >= _editRole);
}

void RamObjectWidget::edit()
{
    if (_hasEditWidget) _dockEditWidget->show();
}

void RamObjectWidget::setupUi()
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setSpacing(3);
    layout->setContentsMargins(3,3,3,3);

    icon = new QLabel(this);
    layout->addWidget(icon);

    title = new QLabel(this);
    layout->addWidget(title);

    layout->addStretch();

    editButton = new QToolButton(this);
    editButton->setIcon( QIcon(":/icons/edit") );
    editButton->setIconSize(QSize(12,12));
    editButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
    layout->addWidget(editButton);

    this->setLayout(layout);

    this->setMinimumHeight(30);
}
