#include "objecteditwidget.h"

ObjectEditWidget::ObjectEditWidget(QWidget *parent) :
    QScrollArea(parent)
{
    QWidget *dummy = new QWidget(this);
    setupUi(dummy);
    this->setWidget(dummy);
    this->setWidgetResizable(true);
    this->setFrameStyle(QFrame::NoFrame);

    setObject(nullptr);

    connectEvents();
}

ObjectEditWidget::ObjectEditWidget(RamObject *o, QWidget *parent) :
    QScrollArea(parent)
{
    QWidget *dummy = new QWidget(this);
    setupUi(dummy);
    this->setWidget(dummy);
    this->setWidgetResizable(true);
    this->setFrameStyle(QFrame::NoFrame);

    setObject(o);

    connectEvents();
}

RamObject *ObjectEditWidget::object() const
{
    return _object;
}

void ObjectEditWidget::hideName(bool hide)
{
    nameLabel->setVisible(!hide);
    nameEdit->setVisible(!hide);
    shortNameLabel->setVisible(!hide);
    shortNameEdit->setVisible(!hide);
}

void ObjectEditWidget::hideStatus(bool hide)
{
    statusLabel->setVisible(!hide);
}

void ObjectEditWidget::setObject(RamObject *object)
{
    // disconnect all
    while(_objectConnections.count() > 0) disconnect( _objectConnections.takeLast() );

    _object = object;

    nameEdit->setText("");
    shortNameEdit->setText("");
    statusLabel->setText("");

    if (!object) return;

    nameEdit->setText(object->name());
    shortNameEdit->setText(object->shortName());

    _objectConnections << connect( object, &RamObject::removed, this, &ObjectEditWidget::objectRemoved);
    _objectConnections << connect( object, &RamObject::changed, this, &ObjectEditWidget::objectChanged);
}

void ObjectEditWidget::update()
{
    if (!_object) return;

    if (!checkInput()) return;

    updating = true;

    _object->setName(nameEdit->text());
    _object->setShortName(shortNameEdit->text());

    _object->update();

    updating = false;
}

bool ObjectEditWidget::checkInput()
{
    if (!_object) return false;

    if (shortNameEdit->text() == "")
    {
        statusLabel->setText("Short name cannot be empty!");
        return false;
    }

    statusLabel->setText("");
    return true;
}

void ObjectEditWidget::objectRemoved(RamObject *o)
{
    Q_UNUSED(o);
    setObject(nullptr);
}

void ObjectEditWidget::objectChanged(RamObject *o)
{
    if (updating) return;
    Q_UNUSED(o);
    setObject(_object);
}

void ObjectEditWidget::connectEvents()
{
    connect(shortNameEdit, &QLineEdit::textChanged, this, &ObjectEditWidget::checkInput);
    connect(shortNameEdit, &QLineEdit::editingFinished, this, &ObjectEditWidget::update);
    connect(nameEdit, &QLineEdit::editingFinished, this, &ObjectEditWidget::update);
}
