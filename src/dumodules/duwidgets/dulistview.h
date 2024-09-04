#ifndef DULISTVIEW_H
#define DULISTVIEW_H

#include <QListView>

class DuListView : public QListView
{
    Q_OBJECT
public:
    DuListView(QWidget *parent);

protected:
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;

signals:
    void middleClicked(const QModelIndex &);

private:
    // UI Events
    QPoint m_initialDragPos;
    bool m_dragging = false;
    bool m_middlePressed = false;

};

#endif // DULISTVIEW_H
