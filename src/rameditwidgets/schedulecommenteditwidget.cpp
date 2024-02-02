#include "schedulecommenteditwidget.h"

ScheduleCommentEditWidget::ScheduleCommentEditWidget(
    RamProject *project,
    QDateTime date,
    RamScheduleComment *comment,
    QWidget *parent
    ):
        DuScrollArea(parent),
        m_project(project),
        m_comment(comment),
        m_date(date)
{
    setupUi();
    connectEvents();
}

void ScheduleCommentEditWidget::setColor(const QColor &color)
{
    if (!m_comment) {
        m_comment = new RamScheduleComment( m_project );
        m_comment->setDate(m_date);
    }
    m_comment->setColor(color);
}

void ScheduleCommentEditWidget::setComment()
{
    QString comment = ui_commentEdit->toMarkdown();
    if (!m_comment) {
        m_comment = new RamScheduleComment( m_project );
        m_comment->setDate(m_date);
    }
    m_comment->setComment( comment );
}

void ScheduleCommentEditWidget::setupUi()
{
    auto mainWidget = new QWidget(this);
    mainWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->setWidget(mainWidget);
    this->setWidgetResizable(true);
    this->setFrameStyle(QFrame::NoFrame);

    auto *dummyLayout = new QVBoxLayout(mainWidget);
    dummyLayout->setContentsMargins(3,3,3,3);
    dummyLayout->setSpacing(3);

    auto mainLayout = new QFormLayout();
    mainLayout->setSpacing(3);
    mainLayout->setContentsMargins(0,0,0,0);
    dummyLayout->addLayout(mainLayout);

    QString ampm;
    if ( m_date.time().hour() >= 12 ) ampm = "pm";
    else ampm = "am";
    ui_dateLabel = new QLabel(
        "<b>" + m_date.toString("yyyy-MM-dd ") + ampm + "</b>"
        );
    mainLayout->addRow("Date", ui_dateLabel );

    ui_colorSelector = new DuQFColorSelector(mainWidget);
    mainLayout->addRow("Color", ui_colorSelector );
    if (m_comment) ui_colorSelector->setColor( m_comment->color() );

    ui_commentEdit = new DuQFTextEdit(mainWidget);
    ui_commentEdit->setProperty("class", "duBlock");
    ui_commentEdit->setMinimumHeight(30);
    ui_commentEdit->setPlaceholderText(tr("Comment..."));
    ui_commentEdit->setObjectName("commentEdit");
    ui_commentEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    dummyLayout->addWidget(ui_commentEdit);
    if (m_comment)
        ui_commentEdit->setMarkdown(m_comment->comment());
}

void ScheduleCommentEditWidget::connectEvents()
{
    connect(ui_colorSelector, &DuQFColorSelector::colorChanged,
            this, &ScheduleCommentEditWidget::setColor);

    connect( ui_commentEdit, &DuQFTextEdit::editingFinished,
            this, &ScheduleCommentEditWidget::setComment);
}
