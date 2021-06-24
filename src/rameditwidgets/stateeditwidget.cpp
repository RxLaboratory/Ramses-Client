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

    QSignalBlocker b1(colorEdit);
    QSignalBlocker b2(completionSpinBox);

    ObjectEditWidget::setObject(state);
    _state = state;

    colorEdit->setText("");
    colorEdit->setStyleSheet("");
    completionSpinBox->setValue(50);

    if (!state) return;

    colorEdit->setText(state->color().name());
    updateColorEditStyle();
    completionSpinBox->setValue(state->completionRatio());

    this->setEnabled(Ramses::instance()->isAdmin());
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

void StateEditWidget::updateColorEditStyle()
{
    if (colorEdit->text().count() == 7)
    {
        QColor c(colorEdit->text());
        QString style = "background-color: " + c.name() + ";";
        if (c.lightness() > 80) style += "color: #232323;";
        colorEdit->setStyleSheet(style);
    }
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
        update();
    }
    this->setEnabled(true);
}

void StateEditWidget::setupUi()
{
    QLabel *colorLabel = new QLabel("Color", this);
    ui_mainFormLayout->addWidget(colorLabel, 3, 0);

    QHBoxLayout *colorLayout = new QHBoxLayout();
    colorLayout->setSpacing(3);
    colorLayout->setContentsMargins(0,0,0,0);

    colorEdit = new QLineEdit(this);
    colorLayout->addWidget(colorEdit);

    colorButton = new QToolButton(this);
    colorButton->setIcon(QIcon(":/icons/color-dialog"));
    colorLayout->addWidget(colorButton);

    ui_mainFormLayout->addLayout(colorLayout, 3, 1);

    QLabel *completionLabel = new QLabel("Completion ratio", this);
    ui_mainFormLayout->addWidget(completionLabel, 4, 0);

    completionSpinBox = new DuQFSpinBox(this);
    completionSpinBox->setSuffix("%");
    completionSpinBox->setMaximumHeight(completionLabel->height());
    ui_mainFormLayout->addWidget(completionSpinBox, 4, 1);

    ui_mainLayout->addStretch();
}

void StateEditWidget::connectEvents()
{
    connect(colorEdit, SIGNAL(editingFinished()), this, SLOT(updateColorEditStyle()));
    connect(colorEdit, SIGNAL(editingFinished()), this, SLOT(update()));
    connect(colorButton, SIGNAL(clicked()), this, SLOT(selectColor()));
    connect(completionSpinBox, &DuQFSpinBox::valueChanged, this, &StateEditWidget::update);
}
