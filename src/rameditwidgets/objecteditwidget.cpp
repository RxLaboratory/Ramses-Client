#include "objecteditwidget.h"

ObjectEditWidget::ObjectEditWidget(QWidget *parent) :
    QScrollArea(parent)
{
    setupUi();

    setObject(nullptr);

    connectEvents();
}

ObjectEditWidget::ObjectEditWidget(RamObject *o, QWidget *parent) :
    QScrollArea(parent)
{
    setupUi();

    setObject(o);

    connectEvents();
}

RamObject *ObjectEditWidget::object() const
{
    return m_object;
}

void ObjectEditWidget::hideName(bool hide)
{
    ui_nameLabel->setVisible(!hide);
    ui_nameEdit->setVisible(!hide);
    ui_shortNameLabel->setVisible(!hide);
    ui_shortNameEdit->setVisible(!hide);
}

void ObjectEditWidget::monitorDbQuery(QString queryName)
{
    m_dbQueries << queryName;
}

void ObjectEditWidget::setObject(RamObject *object)
{
    // disconnect all
    while(_objectConnections.count() > 0)
    {
        disconnect( _objectConnections.takeLast() );
    }

    m_object = object;

    QSignalBlocker b1(ui_nameEdit);
    QSignalBlocker b2(ui_shortNameEdit);
    QSignalBlocker b3(ui_commentEdit);

    ui_nameEdit->setText("");
    ui_shortNameEdit->setText("");
    ui_commentEdit->setText("");

    if (!object) return;

    ui_nameEdit->setText(object->name());
    ui_shortNameEdit->setText(object->shortName());
    ui_commentEdit->setText(object->comment());

    ui_shortNameEdit->setEnabled( !m_dontRename.contains(object->shortName()) );

    // If the folder already exists, freeze the ID
    if ( object->path() != "" &&  QFileInfo::exists( object->path() ) && object->shortName() != "NEW" ) ui_shortNameEdit->setEnabled(false);

    _objectConnections << connect( object, &RamObject::removed, this, &ObjectEditWidget::objectRemoved);
    _objectConnections << connect( object, &RamObject::changed, this, &ObjectEditWidget::objectChanged);
}

void ObjectEditWidget::update()
{
    if (!checkInput()) return;

    updating = true;

    m_object->setName(ui_nameEdit->text());
    m_object->setShortName(ui_shortNameEdit->text());
    m_object->setComment(ui_commentEdit->toPlainText());

    m_object->update();

    updating = false;
}

bool ObjectEditWidget::checkInput()
{
    if (!m_object) return false;


    if (ui_shortNameEdit->text() == "")
    {
        // bug in Qt, signal is fired twice when showing the message box
        if (!ui_shortNameEdit->isModified()) return false;
        ui_shortNameEdit->setModified(false);

        QMessageBox::warning(this, "Missing ID", "You need to set an ID for this item." );
        ui_shortNameEdit->setText(m_object->shortName());
        ui_shortNameEdit->setFocus(Qt::OtherFocusReason);
        return false;
    }

    return true;
}

void ObjectEditWidget::objectRemoved(RamObject *o)
{
    Q_UNUSED(o);
    setObject(nullptr);
}

void ObjectEditWidget::dbiDataReceived(QJsonObject data)
{
    // Only if we're visible!
    if (!this->isVisible()) return;

    // Show error if monitoring & unsuccessful
    if (data.value("success").toBool(false)) return;

    if (!m_dbQueries.contains( data.value("query").toString()) ) return;

    if (!m_modified) return;
    QMessageBox::warning(this, "Server Error", data.value("message").toString() );
}

void ObjectEditWidget::objectChanged(RamObject *o)
{
    if (updating) return;
    Q_UNUSED(o);
    setObject(m_object);
}

void ObjectEditWidget::showEvent(QShowEvent *event)
{
    if(!event->spontaneous()) this->setObject(m_object);
}

bool ObjectEditWidget::eventFilter(QObject *obj, QEvent *event)
{
    if (obj->objectName() == "commentEdit")
        if (event->type() == QEvent::FocusOut)
        {
            update();
            //return true;
        }
    return false;
}

void ObjectEditWidget::setupUi()
{
    QWidget *dummy = new QWidget(this);
    dummy->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

    ui_mainLayout = new QVBoxLayout(dummy);
    ui_mainLayout->setSpacing(3);
    ui_mainLayout->setContentsMargins(3, 3, 3, 3);

    ui_mainFormLayout = new QGridLayout();
    ui_mainFormLayout->setSpacing(3);

    ui_nameLabel = new QLabel("Name", dummy);
    ui_mainFormLayout->addWidget(ui_nameLabel, 0, 0);

    ui_nameEdit = new QLineEdit(dummy);
    ui_mainFormLayout->addWidget(ui_nameEdit, 0, 1);

    ui_shortNameLabel = new QLabel("ID", dummy);
    ui_mainFormLayout->addWidget(ui_shortNameLabel, 1, 0);

    ui_shortNameEdit = new QLineEdit(dummy);
    ui_mainFormLayout->addWidget(ui_shortNameEdit, 1, 1);

    ui_commentLabel = new QLabel("Comment", dummy);
    ui_mainFormLayout->addWidget(ui_commentLabel, 2, 0);

    ui_commentEdit = new QTextEdit(dummy);
    ui_commentEdit->setMaximumHeight(50);
    ui_commentEdit->setObjectName("commentEdit");
    ui_mainFormLayout->addWidget(ui_commentEdit, 2, 1);

    ui_mainLayout->addLayout(ui_mainFormLayout);

    this->setWidget(dummy);
    this->setWidgetResizable(true);
    this->setFrameStyle(QFrame::NoFrame);
}

void ObjectEditWidget::connectEvents()
{
    connect(ui_shortNameEdit, &QLineEdit::editingFinished, this, &ObjectEditWidget::update);
    connect(ui_nameEdit, &QLineEdit::editingFinished, this, &ObjectEditWidget::update);
    ui_commentEdit->installEventFilter(this);
    connect(DBInterface::instance(), SIGNAL(data(QJsonObject)), this, SLOT(dbiDataReceived(QJsonObject)));
}
