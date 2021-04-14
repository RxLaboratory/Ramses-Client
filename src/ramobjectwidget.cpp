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

void RamObjectWidget::disableEdit()
{
    setEditWidget(nullptr);
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
    if (!w)
    {
        _hasEditWidget = false;
        userChanged();
        return;
    }
    w->setParent(_dockEditWidget);
    _dockEditWidget->setWidget(w);
    _hasEditWidget = true;
    userChanged();
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
    editButton->setVisible( u->role() >= _editRole && _hasEditWidget);
}

void RamObjectWidget::edit()
{
    if (_hasEditWidget) _dockEditWidget->show();
}

void RamObjectWidget::setupUi()
{
    QVBoxLayout *vlayout = new QVBoxLayout();
    vlayout->setContentsMargins(0,3,0,3);
    vlayout->setSpacing(0);

    // include in a frame for the BG
    QFrame *mainFrame = new QFrame(this);

    QHBoxLayout *layout = new QHBoxLayout();
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

    mainFrame->setLayout(layout);
    vlayout->addWidget(mainFrame);
    this->setLayout(vlayout);

    this->setMinimumHeight(30);
}

bool RamObjectWidget::selected() const
{
    return _selected;
}

void RamObjectWidget::setSelected(bool selected)
{
    _selected = selected;

    if (selected)
    {
         QString bgcol = DuUI::getColor("abyss-grey").name();
         QString col = DuUI::getColor("light-grey").name();
         QString css = "QFrame {";
         css += "background-color: " + bgcol + ";";
         css += "color: " + col + ";";
         css += "}";
         this->setStyleSheet(css);
    }
    else
    {
        this->setStyleSheet("");
    }
}

void RamObjectWidget::select()
{
    setSelected(true);
}
