#include "stateeditwidget.h"

StateEditWidget::StateEditWidget(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);

    completionSpinBox = new DuQFSpinBox(this);
    completionSpinBox->setSuffix("%");
    completionLayout->addWidget(completionSpinBox);

    connect(updateButton, SIGNAL(clicked()), this, SLOT(update()));
    connect(revertButton, SIGNAL(clicked()), this, SLOT(revert()));
    connect(colorEdit, SIGNAL(editingFinished()), this, SLOT(updateColorEditStyle()));
    connect(colorButton, SIGNAL(clicked()), this, SLOT(selectColor()));
    connect(shortNameEdit, &QLineEdit::textChanged, this, &StateEditWidget::checkInput);
    connect(DBInterface::instance(),&DBInterface::log, this, &StateEditWidget::dbiLog);

    this->setEnabled(false);
}

RamState *StateEditWidget::state() const
{
    return _state;
}

void StateEditWidget::setState(RamState *state)
{
    disconnect(_currentStateConnection);

    _state = state;
    nameEdit->setText("");
    shortNameEdit->setText("");
    colorEdit->setText("");
    colorEdit->setStyleSheet("");
    completionSpinBox->setValue(50);

    if (!state) return;

    nameEdit->setText(state->name());
    shortNameEdit->setText(state->shortName());
    QString color = state->color().name();
    colorEdit->setText(state->color().name());
    updateColorEditStyle();
    completionSpinBox->setValue(state->completionRatio());

    this->setEnabled(Ramses::instance()->isAdmin());

    _currentStateConnection = connect(state, &RamStep::destroyed, this, &StateEditWidget::stateDestroyed);
}

void StateEditWidget::update()
{
    if (!_state) return;

    this->setEnabled(false);

    if (!checkInput())
    {
        this->setEnabled(true);
        return;
    }

    _state->setName(nameEdit->text());
    _state->setShortName(shortNameEdit->text());
    _state->setColor(QColor(colorEdit->text()));
    _state->setCompletionRatio(completionSpinBox->value());

    _state->update();

    this->setEnabled(true);
}

void StateEditWidget::revert()
{
    setState(_state);
}

bool StateEditWidget::checkInput()
{
    if (!_state) return false;

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

void StateEditWidget::stateDestroyed(QObject *o)
{
    setState(nullptr);
}

void StateEditWidget::dbiLog(QString m, LogUtils::LogType t)
{
    if (t != LogUtils::Remote && t != LogUtils::Debug) statusLabel->setText(m);
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
    }
    this->setEnabled(true);
}
