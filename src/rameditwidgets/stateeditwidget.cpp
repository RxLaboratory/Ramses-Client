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

    QSignalBlocker b1(ui_colorSelector);
    QSignalBlocker b2(completionSpinBox);

    ObjectEditWidget::setObject(state);
    _state = state;

    ui_colorSelector->setColor(QColor(25,25,25));
    completionSpinBox->setValue(50);

    if (!state) return;

    ui_colorSelector->setColor(state->color());
    completionSpinBox->setValue(state->completionRatio());

    this->setEnabled(Ramses::instance()->isAdmin());
}

void StateEditWidget::update()
{
    if (!_state) return;

    updating = true;

    _state->setColor(ui_colorSelector->color());
    _state->setCompletionRatio(completionSpinBox->value());

    ObjectEditWidget::update();

    updating = false;
}

void StateEditWidget::setupUi()
{
    QLabel *colorLabel = new QLabel("Color", this);
    ui_mainFormLayout->addWidget(colorLabel, 3, 0);

    ui_colorSelector = new DuQFColorSelector(this);

    ui_mainFormLayout->addWidget(ui_colorSelector, 3, 1);

    QLabel *completionLabel = new QLabel("Completion ratio", this);
    ui_mainFormLayout->addWidget(completionLabel, 4, 0);

    completionSpinBox = new DuQFSpinBox(this);
    completionSpinBox->setSuffix("%");
    completionSpinBox->setMaximumHeight(completionLabel->height());
    ui_mainFormLayout->addWidget(completionSpinBox, 4, 1);

    ui_mainLayout->addStretch();

    ui_mainLayout->setStretch(0,0);
    ui_mainLayout->setStretch(1,0);
    ui_mainLayout->setStretch(2,100);

    m_dontRename << "TODO" << "STB" << "NO" << "WIP" << "OK";

    ui_mainLayout->addStretch();
}

void StateEditWidget::connectEvents()
{
    connect(completionSpinBox, &DuQFSpinBox::valueChanged, this, &StateEditWidget::update);
    connect(ui_colorSelector, SIGNAL(colorChanged(QColor)), this, SLOT(update()));

    monitorDbQuery("updateState");
}
