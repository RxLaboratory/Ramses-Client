#include "stateeditwidget.h"

StateEditWidget::StateEditWidget(RamState *state, QWidget *parent) :
    ObjectEditWidget(state, parent)
{
    setupUi();
    connectEvents();

    setObject(state);
}

StateEditWidget::StateEditWidget(QWidget *parent) :
    ObjectEditWidget(parent)
{
    setupUi();
    connectEvents();

    setObject(nullptr);

    this->setEnabled(false);
}

RamState *StateEditWidget::state() const
{
    return _state;
}

void StateEditWidget::setObject(RamObject *obj)
{
    RamState *state = (RamState*)obj;

    this->setEnabled(false);

    ObjectEditWidget::setObject(state);
    _state = state;

    QSignalBlocker b1(colorEdit);

    colorEdit->setText("");
    colorEdit->setStyleSheet("");
    completionSpinBox->setValue(50);

    if (!state) return;

    colorEdit->setText(state->color().name());
    updateColorEditStyle();
    completionSpinBox->setValue(state->completionRatio());

    this->setEnabled(Ramses::instance()->isAdmin());

    _objectConnections << connect(state, &RamState::changed, this, &StateEditWidget::stateChanged);
}

void StateEditWidget::update()
{
    if (!_state) return;

    updating = true;

    _state->setColor(QColor(colorEdit->text()));
    _state->setCompletionRatio(completionSpinBox->value());

    ObjectEditWidget::update();

    updating = false;
}

void StateEditWidget::stateChanged(RamObject *o)
{
    if (updating) return;
    Q_UNUSED(o);
    setObject(_state);
}

void StateEditWidget::updateColorEditStyle()
{
    if (colorEdit->text().count() == 7)
    {
        QColor c(colorEdit->text());
        QString style = "background-color: " + c.name() + ";";
        if (c.lightness() > 80) style += "color: #232323;";
        colorEdit->setStyleSheet(style);
    }
    update();
}

void StateEditWidget::selectColor()
{
    this->setEnabled(false);
    QColorDialog cd(QColor(_state->color()));
    cd.setOptions(QColorDialog::DontUseNativeDialog);
    //cd.setWindowFlags(Qt::FramelessWindowHint);
    //cd.move(this->parentWidget()->parentWidget()->geometry().center().x()-cd.geometry().width()/2,this->parentWidget()->parentWidget()->geometry().center().y()-cd.geometry().height()/2);
    if (cd.exec())
    {
        QColor color = cd.selectedColor();
        colorEdit->setText(color.name());
        updateColorEditStyle();
    }
    this->setEnabled(true);
}

void StateEditWidget::setupUi()
{
    QLabel *colorLabel = new QLabel("Color", this);
    mainFormLayout->addWidget(colorLabel, 2, 0);

    QHBoxLayout *colorLayout = new QHBoxLayout();
    colorLayout->setSpacing(3);
    colorLayout->setContentsMargins(0,0,0,0);

    colorEdit = new QLineEdit(this);
    colorLayout->addWidget(colorEdit);

    colorButton = new QToolButton(this);
    colorButton->setIcon(QIcon(":/icons/color-dialog"));
    colorLayout->addWidget(colorButton);

    mainFormLayout->addLayout(colorLayout, 2, 1);

    QLabel *completionLabel = new QLabel("Completion ratio", this);
    mainFormLayout->addWidget(completionLabel, 3, 0);

    completionSpinBox = new DuQFSpinBox(this);
    completionSpinBox->setSuffix("%");
    completionSpinBox->setMaximumHeight(completionLabel->height());
    mainFormLayout->addWidget(completionSpinBox, 3, 1);

    mainLayout->addStretch();
}

void StateEditWidget::connectEvents()
{
    connect(colorEdit, SIGNAL(editingFinished()), this, SLOT(updateColorEditStyle()));
    connect(colorButton, SIGNAL(clicked()), this, SLOT(selectColor()));
    connect(completionSpinBox, &DuQFSpinBox::valueChanged, this, &StateEditWidget::update);
}
