#include "ramobjectwidget.h"

#include "mainwindow.h"

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
    delete dockEditWidget();
}

RamObject *RamObjectWidget::ramObject() const
{
    return m_object;
}

void RamObjectWidget::setObject(RamObject *o)
{
    this->setEnabled(false);
    while(!m_objectConnections.isEmpty()) disconnect(m_objectConnections.takeLast());
    m_object = o;
    setTitle("");
    if (!o) return;

    objectChanged();

    m_objectConnections << connect(m_object, &RamObject::changed, this, &RamObjectWidget::setObject);
    m_objectConnections << connect(m_object, &RamObject::removed, this, &RamObjectWidget::objectRemoved);

    setEnabled(true);
}

void RamObjectWidget::setEditable(bool editable)
{
    if (!_hasEditWidget) editable = false;
    if (!_hasEditRights) editable = false;
    _editable = editable;
    editButton->setVisible( editable );
}

void RamObjectWidget::setRemovable(bool removable)
{
    removeButton->setVisible( removable );
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
    icon->setPixmap(QPixmap(i));
    _dockEditWidget->setIcon(i);
}

void RamObjectWidget::setColor(QColor color)
{
    if (color.lightness() < 80) _color = color.lighter(400);
    else _color = color;
    setSelected(_selected);
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

    MainWindow *mw = (MainWindow*)GuiUtils::appMainWindow();
    mw->addObjectDockWidget(_dockEditWidget);

    userChanged();
}

void RamObjectWidget::objectRemoved()
{
    deleteLater();
}

void RamObjectWidget::userChanged()
{
    _hasEditRights = false;
    RamUser *u = Ramses::instance()->currentUser();
    if (!u) return;
    _hasEditRights = u->role() >= _editRole && _hasEditWidget ;
    if ( _editUsers.contains(u) ) _hasEditRights = true;
}

void RamObjectWidget::objectChanged()
{
    QString t = m_object->name();
    if ( t == "" ) t = m_object->shortName();
    setTitle(t);

    if (m_object->comment() == "") commentEdit->hide();
    else commentEdit->show();
    commentEdit->setPlainText( m_object->comment() );
}

void RamObjectWidget::remove()
{
    QMessageBox::StandardButton confirm = QMessageBox::question(this,
                                                                "Confirm deletion",
                                                                "Are you sure you want to premanently remove " + m_object->shortName() + " - " + m_object->name() + "?" );
    if (confirm != QMessageBox::Yes) return;
    m_object->remove();
}

void RamObjectWidget::edit()
{
    if (_hasEditWidget && _editable && _hasEditRights) _dockEditWidget->show();
}

void RamObjectWidget::setupUi()
{
    QVBoxLayout *vlayout = new QVBoxLayout(this);
    vlayout->setContentsMargins(0,0,0,3);
    vlayout->setSpacing(0);

    // include in a frame for the BG
    QFrame *mainFrame = new QFrame(this);

    QVBoxLayout *mLayout = new QVBoxLayout(mainFrame);
    mLayout->setSpacing(3);
    mLayout->setContentsMargins(3,3,3,3);

    // --- TITLE BAR & BUTTONS ---

    QHBoxLayout *titleLayout = new QHBoxLayout();
    titleLayout->setSpacing(3);
    titleLayout->setContentsMargins(12,0,0,0);

    icon = new QLabel(this);
    icon->setScaledContents(true);
    icon->setFixedSize(12,12);
    titleLayout->addWidget(icon);

    title = new QLabel(this);
    titleLayout->addWidget(title);

    titleLayout->addStretch();

    editButton = new QToolButton(this);
    editButton->setIcon( QIcon(":/icons/edit") );
    editButton->setIconSize(QSize(10,10));
    editButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
    titleLayout->addWidget(editButton);

    buttonsLayout = new QHBoxLayout();
    buttonsLayout->setContentsMargins(0,0,0,0);
    buttonsLayout->setSpacing(3);

    exploreButton = new QToolButton(this);
    exploreButton->setObjectName(QStringLiteral("exploreButton"));
    exploreButton->setIcon(QIcon(":/icons/reveal-folder-s"));
    exploreButton->setIconSize(QSize(10,10));
    exploreButton->hide();
    buttonsLayout->addWidget(exploreButton);

    titleLayout->addLayout(buttonsLayout);

    removeButton = new QToolButton(this);
    removeButton->setIcon( QIcon(":/icons/remove") );
    removeButton->setIconSize(QSize(10,10));
    removeButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
    removeButton->setVisible(false);
    titleLayout->addWidget(removeButton);

    mLayout->addLayout(titleLayout);

    // --- PRIMARY CONTENT ---

    primaryContentWidget = new QWidget(this);
    primaryContentWidget->setStyleSheet("background-color: rgba(0,0,0,0);");
    primaryContentLayout = new QVBoxLayout(primaryContentWidget);
    primaryContentLayout->setSpacing(3);
    primaryContentLayout->setContentsMargins(0,0,0,0);
    primaryContentWidget->hide();
    mLayout->addWidget(primaryContentWidget);

    // --- SECONDARY CONTENT ---

    secondaryContentWidget = new QWidget(this);
    secondaryContentWidget->hide();
    secondaryContentWidget->setStyleSheet("background-color: rgba(0,0,0,0);");
    secondaryContentLayout = new QVBoxLayout(secondaryContentWidget);
    secondaryContentLayout->setSpacing(3);
    secondaryContentLayout->setContentsMargins(0,0,0,0);

    commentEdit = new QPlainTextEdit(this);
    commentEdit->setReadOnly(true);
    commentEdit->setMaximumHeight(100);
    commentEdit->setTextInteractionFlags(Qt::NoTextInteraction);
    QString style = "background-color: rgba(0,0,0,0);";
    commentEdit->setStyleSheet(style);

    secondaryContentLayout->addWidget(commentEdit);
    secondaryContentLayout->addStretch();

    mLayout->addWidget(secondaryContentWidget);

    // --- FINISH ---

    vlayout->addWidget(mainFrame);

    this->setMinimumHeight(30);
}

void RamObjectWidget::connectEvents()
{
    connect(Ramses::instance(), &Ramses::loggedIn, this, &RamObjectWidget::userChanged);
    connect(editButton, SIGNAL(clicked()), this, SLOT(edit()));
    connect(removeButton, SIGNAL(clicked()), this, SLOT(remove()));
    connect(exploreButton, SIGNAL(clicked()), this, SLOT(exploreClicked()));
}

bool RamObjectWidget::alwaysShowPrimaryContent() const
{
    return m_alwaysShowPrimaryContent;
}

void RamObjectWidget::setAlwaysShowPrimaryContent(bool newAlwaysShowPrimaryContent)
{
    m_alwaysShowPrimaryContent = newAlwaysShowPrimaryContent;
}

void RamObjectWidget::showExploreButton(bool s)
{
    exploreButton->setVisible(s);
}

int RamObjectWidget::primaryContentHeight() const
{
    return m_primaryContentHeight;
}

void RamObjectWidget::setPrimaryContentHeight(int newPrimaryContentHeight)
{
    m_primaryContentHeight = newPrimaryContentHeight;
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
        if (!_color.isValid())
        {
            this->setStyleSheet("");
        }
        else
        {
            QString col = _color.name();
            QString css = "QFrame {";
            css += "color: " + col + ";";
            css += "}";
            this->setStyleSheet(css);
        }
    }
}

void RamObjectWidget::select()
{
    setSelected(true);
}

void RamObjectWidget::resizeEvent(QResizeEvent *event)
{
    if (!this->isVisible()) return;
    QWidget::resizeEvent(event);
    /*// If size is too small, just hide content
    int h = event->size().height();

    secondaryContentWidget->hide();

    if (!m_alwaysShowPrimaryContent)
    {
        primaryContentWidget->hide();
        if (h < 30 + m_primaryContentHeight) return;
        if (m_primaryContentHeight > 0) primaryContentWidget->show();
    }

    if (h < 70 + m_primaryContentHeight || commentEdit->toPlainText() == "") return;

    secondaryContentWidget->show();//*/

}

void RamObjectWidget::explore(QString path)
{
    if (path == "") return;
    if(!QFileInfo::exists(path))
    {
        QMessageBox::StandardButton rep = QMessageBox::question(this,
                                                                "The folder does not exist",
                                                                "This folder:\n\n" + path + "\n\ndoes not exist yet.\nDo you want to create it now?",
                                                                QMessageBox::Yes | QMessageBox::No,
                                                                QMessageBox::Yes);
        if (rep == QMessageBox::Yes) QDir(path).mkpath(".");
    }
    FileUtils::openInExplorer( path );
}

void RamObjectWidget::adjustCommentEditSize()
{
    // Get text height (returns the number of lines and not the actual height in pixels
    int docHeight = commentEdit->document()->size().toSize().height();
    // Compute needed height in pixels
    int h = docHeight * ( commentEdit->fontMetrics().height() ) + commentEdit->fontMetrics().height() * 2;
    commentEdit->setMaximumHeight(h);
}
