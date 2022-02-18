#include "duqftextedit.h"

DuQFTextEdit::DuQFTextEdit(const QString &text, QWidget *parent):
    QTextEdit(text, parent)
{
    connectEvents();
}

DuQFTextEdit::DuQFTextEdit(QWidget *parent):
    QTextEdit(parent)
{
    connectEvents();
}

void DuQFTextEdit::focusOutEvent(QFocusEvent *event)
{
    Q_UNUSED(event);
    finishEditing();
    QTextEdit::focusOutEvent(event);
}

void DuQFTextEdit::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Enter && m_captureEnterKey)
    {
        emit enterPressed();
        finishEditing();
        return;
    }
    QTextEdit::keyPressEvent(event);
}

void DuQFTextEdit::finishEditing()
{
    if (m_changed) {
        this->setMarkdown( this->toMarkdown() );
        emit editingFinished();
    }
    m_changed = false;
}

void DuQFTextEdit::setTextChanged()
{
    m_changed = true;
}

void DuQFTextEdit::connectEvents()
{
    connect(this, SIGNAL(textChanged()), this, SLOT(setTextChanged()));
}

bool DuQFTextEdit::captureEnterKey() const
{
    return m_captureEnterKey;
}

void DuQFTextEdit::setCaptureEnterKey(bool newCaptureEnterKey)
{
    m_captureEnterKey = newCaptureEnterKey;
}
