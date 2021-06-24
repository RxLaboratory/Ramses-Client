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
    ui_statusCommentEdit->setPlainText("");
    if (!status) return;

    ui_stateBox->setObject(status->state());
    ui_completionBox->setValue(status->completionRatio());
    ui_versionBox->setValue(status->version());
    ui_statusCommentEdit->setPlainText(status->comment());
}

RamState *StatusEditWidget::state() const
{
    return qobject_cast<RamState*>( ui_stateBox->currentObject() );
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
    return ui_statusCommentEdit->toPlainText();
}

void StatusEditWidget::currentStateChanged(RamObject *stateObj)
{
    RamState *state = qobject_cast<RamState*>(stateObj);
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
    emit statusUpdated(state(), ui_completionBox->value(), ui_versionBox->value(), ui_statusCommentEdit->toPlainText() );
}

void StatusEditWidget::adjustCommentEditSize()
{
    // Get text height (returns the number of lines and not the actual height in pixels
    int docHeight = ui_statusCommentEdit->document()->size().toSize().height();
    // Compute needed height in pixels
    int h = docHeight * ( ui_statusCommentEdit->fontMetrics().height() ) + ui_statusCommentEdit->fontMetrics().height() * 2;
    //commentEdit->resize(commentEdit->width(), h);
    ui_statusCommentEdit->setMaximumHeight(h);
}

void StatusEditWidget::revert()
{
    setStatus(m_status);
}

void StatusEditWidget::setupUi()
{
    this->hideName();
    ui_commentEdit->hide();
    ui_commentLabel->hide();

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

    ui_statusCommentEdit = new QPlainTextEdit(this);
    ui_statusCommentEdit->setPlaceholderText("Comment...");
    ui_statusCommentEdit->setMinimumHeight(30);
    ui_statusCommentEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui_mainLayout->insertWidget(1, ui_statusCommentEdit);

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
    connect(ui_statusCommentEdit, &QPlainTextEdit::textChanged, this, &StatusEditWidget::adjustCommentEditSize);
    connect(ui_stateBox, SIGNAL(currentObjectChanged(RamObject*)), this, SLOT(currentStateChanged(RamObject*)));
    connect(ui_setButton, &QToolButton::clicked, this, &StatusEditWidget::updateStatus);
    connect(ui_revertButton, &QToolButton::clicked, this, &StatusEditWidget::revert);
}
