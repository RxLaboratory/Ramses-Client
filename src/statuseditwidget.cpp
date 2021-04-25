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
    _status = status;

    stateBox->setCurrentText("STB");
    completionBox->setValue(0);
    versionBox->setValue(1);
    commentEdit->setPlainText("");
    if (!status) return;

    stateBox->setCurrentState(status->state());
    completionBox->setValue(status->completionRatio());
    versionBox->setValue(status->version());
    commentEdit->setPlainText(status->comment());
}

RamState *StatusEditWidget::state() const
{
    return stateBox->currentState();
}

int StatusEditWidget::completionRatio() const
{
    return completionBox->value();
}

int StatusEditWidget::version() const
{
    return versionBox->value();
}

QString StatusEditWidget::comment() const
{
    return commentEdit->toPlainText();
}

void StatusEditWidget::currentStateChanged(RamState *state)
{
    if (state)
    {
        completionBox->setValue(state->completionRatio());
    }
    else
    {
        completionBox->setValue(50);
    }
}

void StatusEditWidget::updateStatus()
{
    emit statusUpdated(stateBox->currentState(), completionBox->value(), versionBox->value(), commentEdit->toPlainText() );
}

void StatusEditWidget::adjustCommentEditSize()
{
    // Get text height (returns the number of lines and not the actual height in pixels
    int docHeight = commentEdit->document()->size().toSize().height();
    // Compute needed height in pixels
    int h = docHeight * ( commentEdit->fontMetrics().height() ) + commentEdit->fontMetrics().height() * 2;
    //commentEdit->resize(commentEdit->width(), h);
    commentEdit->setMaximumHeight(h);
}

void StatusEditWidget::revert()
{
    setStatus(_status);
}

void StatusEditWidget::setupUi()
{
    this->hideName();

    QHBoxLayout *cLayout = new QHBoxLayout();
    cLayout->setContentsMargins(0,0,0,0);
    cLayout->setSpacing(3);

    stateBox = new StateBox(this);
    cLayout->addWidget(stateBox);

    completionBox = new DuQFSpinBox(this);
    completionBox->setMaximum(100);
    completionBox->setMinimum(0);
    completionBox->setSuffix("%");
    completionBox->setValue(50);
    completionBox->setFixedHeight(stateBox->height());
    cLayout->addWidget(completionBox);

    versionBox = new AutoSelectSpinBox(this);
    versionBox->setMaximum(1000);
    versionBox->setValue(1);
    versionBox->setPrefix("v");
    cLayout->addWidget(versionBox);

    cLayout->setStretch(0,0);
    cLayout->setStretch(1,100);
    cLayout->setStretch(2,0);

    mainLayout->insertLayout(0, cLayout);

    commentEdit = new QPlainTextEdit(this);
    commentEdit->setPlaceholderText("Comment...");
    commentEdit->setMinimumHeight(30);
    commentEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mainLayout->insertWidget(1, commentEdit);

    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    buttonsLayout->setSpacing(3);
    buttonsLayout->setContentsMargins(0,0,0,0);

    buttonsLayout->addStretch();

    revertButton = new QToolButton(this);
    revertButton->setText("Revert");
    revertButton->setIcon(QIcon(":/icons/undo"));
    buttonsLayout->addWidget(revertButton);

    setButton = new QToolButton(this);
    setButton->setText("Update");
    setButton->setIcon(QIcon(":/icons/apply"));
    buttonsLayout->addWidget(setButton);

    mainLayout->insertLayout(2,buttonsLayout);

    mainLayout->setStretch(0,0);
    mainLayout->setStretch(1,1);
    mainLayout->setStretch(2,0);

    mainLayout->addStretch();
}

void StatusEditWidget::connectEvents()
{
    connect(commentEdit, &QPlainTextEdit::textChanged, this, &StatusEditWidget::adjustCommentEditSize);
    connect(stateBox, SIGNAL(currentStateChanged(RamState*)), this, SLOT(currentStateChanged(RamState*)));
    connect(setButton, &QToolButton::clicked, this, &StatusEditWidget::updateStatus);
    connect(revertButton, &QToolButton::clicked, this, &StatusEditWidget::revert);
}
