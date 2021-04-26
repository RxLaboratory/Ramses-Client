#include "ramobjectwidget.h"

RamObjectWidget::RamObjectWidget(RamObject *obj, QWidget *parent) : QWidget(parent)
{
    setupUi();

    setObject(obj);
    userChanged();

    // Build a dock widget to edit details
    _dockEditWidget = new ObjectDockWidget(obj);
    _dockEditWidget->hide();

    connectEvents();
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
    _editable = false;
}

void RamObjectWidget::setEditable(bool editable)
{
    _editable = editable;
    editButton->setVisible( editable );
    removeButton->setVisible( editable );
}

void RamObjectWidget::setUserEditRole(RamUser::UserRole role)
{
    _editRole = role;
}

RamObjectList RamObjectWidget::editUsers() const
{
    return _editUsers;
}

void RamObjectWidget::setTitle(QString t)
{
    title->setText(t);
}

void RamObjectWidget::setIcon(QString i)
{
    icon->setPixmap(i);
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
    QFrame *f = new QFrame(_dockEditWidget);
    QVBoxLayout *l = new QVBoxLayout();
    l->setContentsMargins(3,3,3,3);
    l->addWidget(w);
    f->setLayout(l);
    _dockEditWidget->setWidget(f);
    _hasEditWidget = true;
    userChanged();
}

void RamObjectWidget::objectRemoved()
{
    deleteLater();
}

void RamObjectWidget::userChanged()
{
    RamUser *u = Ramses::instance()->currentUser();
    if (!u) return;
    bool ok = u->role() >= _editRole && _hasEditWidget ;
    if ( _editUsers.contains(u) ) ok = true;
    setEditable(ok);
}

void RamObjectWidget::remove()
{
    QMessageBox::StandardButton confirm = QMessageBox::question(this,
                                                                "Confirm deletion",
                                                                "Are you sure you want to premanently remove " + _object->shortName() + " - " + _object->name() + "?" );
    if (confirm != QMessageBox::Yes) return;
    _object->remove();
}

void RamObjectWidget::edit()
{
    if (_hasEditWidget && _editable) _dockEditWidget->show();
}

void RamObjectWidget::setupUi()
{
    QVBoxLayout *vlayout = new QVBoxLayout();
    vlayout->setContentsMargins(0,0,0,3);
    vlayout->setSpacing(0);

    // include in a frame for the BG
    QFrame *mainFrame = new QFrame(this);

    layout = new QVBoxLayout();
    layout->setSpacing(3);
    layout->setContentsMargins(3,3,3,3);

    QHBoxLayout *titleLayout = new QHBoxLayout();
    titleLayout->setSpacing(3);
    titleLayout->setContentsMargins(12,0,0,0);

    icon = new QLabel(this);
    icon->setScaledContents(true);
    icon->setMaximumSize(12,12);
    titleLayout->addWidget(icon);

    title = new QLabel(this);
    titleLayout->addWidget(title);

    titleLayout->addStretch();

    editButton = new QToolButton(this);
    editButton->setIcon( QIcon(":/icons/edit") );
    editButton->setIconSize(QSize(10,10));
    editButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
    titleLayout->addWidget(editButton);

    removeButton = new QToolButton(this);
    removeButton->setIcon( QIcon(":/icons/remove") );
    removeButton->setIconSize(QSize(10,10));
    removeButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
    titleLayout->addWidget(removeButton);

    layout->addLayout(titleLayout);

    mainFrame->setLayout(layout);
    vlayout->addWidget(mainFrame);
    this->setLayout(vlayout);

    this->setMinimumHeight(30);
}

void RamObjectWidget::connectEvents()
{

    connect(_object, &RamObject::changed, this, &RamObjectWidget::setObject);
    connect(_object, &RamObject::removed, this, &RamObjectWidget::objectRemoved);
    connect(Ramses::instance(), &Ramses::loggedIn, this, &RamObjectWidget::userChanged);
    connect(editButton, SIGNAL(clicked()), this, SLOT(edit()));
    connect(removeButton, SIGNAL(clicked()), this, SLOT(remove()));
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
