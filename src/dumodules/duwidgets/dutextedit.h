#ifndef DUTEXTEDIT_H
#define DUTEXTEDIT_H

#include <QTextEdit>

/**
 * @brief The DuTextEdit class
 * @version 1.0.0
 */
class DuTextEdit : public QTextEdit
{
    Q_OBJECT
public:
    DuTextEdit(QWidget *parent = nullptr);

    void setNumLines(int n);

protected:
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;

private:
    // UI Events
    QPoint _initialDragPos;
    bool _dragging = false;
};


#endif // DUTEXTEDIT_H
