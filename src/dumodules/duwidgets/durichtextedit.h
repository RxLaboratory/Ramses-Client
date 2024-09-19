#ifndef DURICHTEXTEDIT_H
#define DURICHTEXTEDIT_H

#include "duwidgets/dumenu.h"
#include <QTextEdit>

/**
 * @brief The DuRichTextEdit class is a standard QTextEdit but with new signals:
 * editingFinished (using focusOut)
 * enterPressed (the keypad enter) if set to capture enter key
 */
class DuRichTextEdit : public QTextEdit
{
    Q_OBJECT
public:
    DuRichTextEdit(const QString &text, QWidget *parent = nullptr);
    DuRichTextEdit(QWidget *parent = nullptr);
    bool captureEnterKey() const;
    void setCaptureEnterKey(bool newCaptureEnterKey);
    void setUseMarkdown(bool use);

public slots:
    void showMarkdown();
    void showRichText();
    void pasteNoFormatting();
signals:
    /**
     * @brief editingFinished To the contrary of QLineEdit::editingFinished,
     * This signal is emitted only if the text has actually changed.
     */
    void editingFinished();
    void enterPressed();

protected:
    void focusOutEvent(QFocusEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;
private slots:
    void finishEditing();
    void setTextChanged();
    void contextMenuHidden();
private:
    void connectEvents();
    void setupUi();
    DuMenu *ui_contextMenu;
    QAction *ui_undo;
    QAction *ui_redo;
    QAction *ui_copy;
    QAction *ui_cut;
    QAction *ui_paste;
    QAction *ui_pasteNoFormatting;
    QAction *ui_selectAll;
    QAction *ui_showSource;
    QAction *ui_validate;

    bool m_changed = false;
    bool m_captureEnterKey = true;
    bool m_freezeValidation = false;
    bool m_useMarkdown = true;
};

#endif // DURICHTEXTEDIT_H
