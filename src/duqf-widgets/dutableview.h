#ifndef DUTABLEVIEW_H
#define DUTABLEVIEW_H

#include <QTableView>

/**
 * @brief The DuTableView class
 * @version 1.0.0
 */
class DuTableView : public QTableView
{
    Q_OBJECT
public:
    DuTableView(QWidget *parent = nullptr);
    void setVerticalHeaderMoveData();
    void setHorizontalHeaderMoveData();

protected:
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;

private slots:
    void moveRowData(int logicalIndex, int oldVisualIndex, int newVisualIndex);
    void moveColumnData(int logicalIndex, int oldVisualIndex, int newVisualIndex);

private:
    // UI Events
    QPoint m_initialDragPos;
    bool m_dragging = false;

    QByteArray m_vheaderState;
    QByteArray m_hheaderState;
};

#endif // DUTABLEVIEW_H
