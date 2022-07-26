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

    checkPath();

    _objectConnections << connect( object, &RamObject::removed, this, &ObjectEditWidget::objectRemoved);
    _objectConnections << connect( object, &RamObject::dataChanged, this, &ObjectEditWidget::objectChanged);
}

void ObjectEditWidget::update()
{
    if (!checkInput()) return;

    updating = true;

    m_object->setName(ui_nameEdit->text());
    m_object->setShortName(ui_shortNameEdit->text());
    m_object->setComment(ui_commentEdit->toMarkdown());

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

void ObjectEditWidget::test()
{
    qDebug() << ui_commentEdit->toPlainText();
}

void ObjectEditWidget::objectChanged(RamObject *o)
{
    if (updating) return;
    Q_UNUSED(o);
    setObject(m_object);
}

void ObjectEditWidget::checkPath()
{
    if(!m_object) return;
    ui_shortNameEdit->setEnabled( !m_dontRename.contains(m_object->shortName()) );
    ui_nameEdit->setEnabled(true);
    // If the folder already exists, freeze the ID or the name (according to specific types)
    if ( m_object->path() != "" &&  QFileInfo::exists( m_object->path() ) && m_object->shortName() != "NEW" )
    {
        if (m_object->objectType() == RamObject::AssetGroup) ui_nameEdit->setEnabled(false);
        else ui_shortNameEdit->setEnabled(false);
    }
}

void ObjectEditWidget::showEvent(QShowEvent *event)
{
    if(!event->spontaneous()) this->setObject(m_object);
}

void ObjectEditWidget::hideEvent(QHideEvent *event)
{
    Q_UNUSED(event)
    update();
}

void ObjectEditWidget::setupUi()
{
    //this->setMaximumWidth(500);

    QWidget *dummy = new QWidget(this);
    dummy->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    ui_mainLayout = new QVBoxLayout(dummy);
    ui_mainLayout->setSpacing(3);
    ui_mainLayout->setContentsMargins(3, 3, 3, 3);

    ui_mainFormLayout = new QGridLayout();
    ui_mainFormLayout->setSpacing(3);

    ui_nameLabel = new QLabel("Name", dummy);
    ui_mainFormLayout->addWidget(ui_nameLabel, 0, 0);

    ui_nameEdit = new QLineEdit(dummy);
    ui_mainFormLayout->addWidget(ui_nameEdit, 0, 1);

    // Name validator
    QRegExp rxn = RegExUtils::getRegExp("name");
    QValidator *nameValidator = new QRegExpValidator(rxn, this);
    ui_nameEdit->setValidator(nameValidator);

    ui_shortNameLabel = new QLabel("ID", dummy);
    ui_mainFormLayout->addWidget(ui_shortNameLabel, 1, 0);

    ui_shortNameEdit = new QLineEdit(dummy);
    ui_mainFormLayout->addWidget(ui_shortNameEdit, 1, 1);

    // Short Name validator
    QRegExp rxsn = RegExUtils::getRegExp("shortname");
    QValidator *shortNameValidator = new QRegExpValidator(rxsn, this);
    ui_shortNameEdit->setValidator(shortNameValidator);

    ui_commentLabel = new QLabel("Comment", dummy);
    ui_mainFormLayout->addWidget(ui_commentLabel, 2, 0);

    ui_commentEdit = new DuQFTextEdit(dummy);
    ui_commentEdit->setMaximumHeight(80);
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
    connect(ui_commentEdit, &DuQFTextEdit::editingFinished, this, &ObjectEditWidget::update);
    connect(DBInterface::instance(), SIGNAL(data(QJsonObject)), this, SLOT(dbiDataReceived(QJsonObject)));
}
