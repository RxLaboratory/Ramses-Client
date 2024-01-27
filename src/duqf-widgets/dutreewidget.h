#ifndef DUTREEWIDGET_H
#define DUTREEWIDGET_H

#include <QTreeWidget>

/**
 * @brief The DuTreeWidget class
 * @version 1.0.0
 */
class DuTreeWidget : public QTreeWidget
{
    Q_OBJECT
public:
    DuTreeWidget(QWidget *parent);

    void addTopLevelItem(QTreeWidgetItem *item); // Override to update geometry as sizehint changes

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

#endif // DUTREEWIDGET_H
