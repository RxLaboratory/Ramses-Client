#ifndef DUQFTEXTEDIT_H
#define DUQFTEXTEDIT_H

#include <QTextEdit>

/**
 * @brief The DuQFTextEdit class is a standard QTextEdit but with new signals:
 * editingFinished (using focusOut)
 * enterPressed (the keypad enter) if set to capture enter key
 */
class DuQFTextEdit : public QTextEdit
{
    Q_OBJECT
public:
    DuQFTextEdit(const QString &text, QWidget *parent = nullptr);
    DuQFTextEdit(QWidget *parent = nullptr);
    bool captureEnterKey() const;
    void setCaptureEnterKey(bool newCaptureEnterKey);

signals:
    void editingFinished();
    void enterPressed();
protected:
    void focusOutEvent(QFocusEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
private slots:
    void finishEditing();
    void setTextChanged();
private:
    void connectEvents();
    bool m_changed = false;
    bool m_captureEnterKey = true;
};

#endif // DUQFTEXTEDIT_H
