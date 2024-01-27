#ifndef DULISTWIDGET_H
#define DULISTWIDGET_H

#include <QListWidget>

/**
 * @brief The DuListWidget class
 * @version 1.0.0
 */
class DuListWidget : public QListWidget
{
    Q_OBJECT
public:
    DuListWidget(QWidget *parent);

    void addItem(QListWidgetItem *item); // Override to update geometry as sizehint changes

    virtual QSize sizeHint() const override;

protected:
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;

private:
    // UI Events
    QPoint m_initialDragPos;
    bool m_dragging = false;

    int verticalSizeHint() const;
};

#endif // DULISTWIDGET_H
