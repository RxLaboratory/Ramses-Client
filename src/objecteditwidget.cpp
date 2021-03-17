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
}

void ObjectEditWidget::update()
{
    qDebug() << checkInput();
    if (!_object) return;

    if (!checkInput()) return;

    _object->setName(nameEdit->text());
    _object->setShortName(shortNameEdit->text());

    _object->update();
}

void ObjectEditWidget::revert()
{
    setObject(nullptr);
}

bool ObjectEditWidget::checkInput()
{
    if (!_object) return false;

    if (shortNameEdit->text() == "")
    {
        statusLabel->setText("Short name cannot be empty!");
        updateButton->setEnabled(false);
        return false;
    }

    statusLabel->setText("");
    updateButton->setEnabled(true);
    return true;
}

void ObjectEditWidget::objectRemoved(RamObject *o)
{
    Q_UNUSED(o);
    setObject(nullptr);
}

void ObjectEditWidget::connectEvents()
{
    connect(updateButton, SIGNAL(clicked()), this, SLOT(update()));
    connect(revertButton, SIGNAL(clicked()), this, SLOT(revert()));
    connect(shortNameEdit, &QLineEdit::textChanged, this, &ObjectEditWidget::checkInput);
}
