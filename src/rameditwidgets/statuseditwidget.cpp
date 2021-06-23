#include "statuseditwidget.h"

StatusEditWidget::StatusEditWidget(QWidget *parent) : ObjectEditWidget( parent)
{
    setupUi();
    connectEvents();
    setStatus(nullptr);
}

StatusEditWidget::StatusEditWidget(RamStatus *status, QWidget *parent) : ObjectEditWidget(status, parent)
{
    setupUi();
    connectEvents();
    setStatus(status);
}

void StatusEditWidget::setStatus(RamStatus *status)
{
    m_status = status;

    ui_stateBox->setCurrentText("STB");
    ui_completionBox->setValue(0);
    ui_versionBox->setValue(1);
    ui_commentEdit->setPlainText("");
    if (!status) return;

    ui_stateBox->setCurrentState(status->state());
    ui_completionBox->setValue(status->completionRatio());
    ui_versionBox->setValue(status->version());
    ui_commentEdit->setPlainText(status->comment());
}

RamState *StatusEditWidget::state() const
{
    return ui_stateBox->currentState();
}

int StatusEditWidget::completionRatio() const
{
    return ui_completionBox->value();
}

int StatusEditWidget::version() const
{
    return ui_versionBox->value();
}

QString StatusEditWidget::comment() const
{
    return ui_commentEdit->toPlainText();
}

void StatusEditWidget::currentStateChanged(RamState *state)
{
    if (state)
    {
        ui_completionBox->setValue(state->completionRatio());
    }
    else
    {
        ui_completionBox->setValue(50);
    }
}

void StatusEditWidget::updateStatus()
{
    emit statusUpdated(ui_stateBox->currentState(), ui_completionBox->value(), ui_versionBox->value(), ui_commentEdit->toPlainText() );
}

void StatusEditWidget::adjustCommentEditSize()
{
    // Get text height (returns the number of lines and not the actual height in pixels
    int docHeight = ui_commentEdit->document()->size().toSize().height();
    // Compute needed height in pixels
    int h = docHeight * ( ui_commentEdit->fontMetrics().height() ) + ui_commentEdit->fontMetrics().height() * 2;
    //commentEdit->resize(commentEdit->width(), h);
    ui_commentEdit->setMaximumHeight(h);
}

void StatusEditWidget::revert()
{
    setStatus(m_status);
}

void StatusEditWidget::setupUi()
{
    this->hideName();

    QHBoxLayout *cLayout = new QHBoxLayout();
    cLayout->setContentsMargins(0,0,0,0);
    cLayout->setSpacing(3);

    ui_stateBox = new StateBox(this);
    cLayout->addWidget(ui_stateBox);

    ui_completionBox = new DuQFSpinBox(this);
    ui_completionBox->setMaximum(100);
    ui_completionBox->setMinimum(0);
    ui_completionBox->setSuffix("%");
    ui_completionBox->setValue(50);
    ui_completionBox->setFixedHeight(ui_stateBox->height());
    cLayout->addWidget(ui_completionBox);

    ui_versionBox = new AutoSelectSpinBox(this);
    ui_versionBox->setMaximum(1000);
    ui_versionBox->setValue(1);
    ui_versionBox->setPrefix("v");
    cLayout->addWidget(ui_versionBox);

    cLayout->setStretch(0,0);
    cLayout->setStretch(1,100);
    cLayout->setStretch(2,0);

    ui_mainLayout->insertLayout(0, cLayout);

    ui_commentEdit = new QPlainTextEdit(this);
    ui_commentEdit->setPlaceholderText("Comment...");
    ui_commentEdit->setMinimumHeight(30);
    ui_commentEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui_mainLayout->insertWidget(1, ui_commentEdit);

    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    buttonsLayout->setSpacing(3);
    buttonsLayout->setContentsMargins(0,0,0,0);

    buttonsLayout->addStretch();

    ui_revertButton = new QToolButton(this);
    ui_revertButton->setText("Revert");
    ui_revertButton->setIcon(QIcon(":/icons/undo"));
    buttonsLayout->addWidget(ui_revertButton);

    ui_setButton = new QToolButton(this);
    ui_setButton->setText("Update");
    ui_setButton->setIcon(QIcon(":/icons/apply"));
    buttonsLayout->addWidget(ui_setButton);

    ui_mainLayout->insertLayout(2,buttonsLayout);

    ui_mainLayout->setStretch(0,0);
    ui_mainLayout->setStretch(1,1);
    ui_mainLayout->setStretch(2,0);

    ui_mainLayout->addStretch();
}

void StatusEditWidget::connectEvents()
{
    connect(ui_commentEdit, &QPlainTextEdit::textChanged, this, &StatusEditWidget::adjustCommentEditSize);
    connect(ui_stateBox, SIGNAL(currentStateChanged(RamState*)), this, SLOT(currentStateChanged(RamState*)));
    connect(ui_setButton, &QToolButton::clicked, this, &StatusEditWidget::updateStatus);
    connect(ui_revertButton, &QToolButton::clicked, this, &StatusEditWidget::revert);
}
