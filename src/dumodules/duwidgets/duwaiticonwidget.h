#ifndef DUWAITICONWIDGET_H
#define DUWAITICONWIDGET_H

#include <QIcon>
#include <QTimer>
#include <QWidget>
#include <QElapsedTimer>

class DuWaitIconWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DuWaitIconWidget(double speed = 1.0, double fps = 24.0, QWidget *parent = nullptr);
    void setSize(const QSize &s) { _size = s; setFixedSize(s); };

protected:
    virtual void showEvent(QShowEvent *) override { _timer.start(); _elapsed.start(); };
    virtual void hideEvent(QHideEvent *) override { _timer.stop(); };
    virtual void paintEvent(QPaintEvent *e) override;

private:
    QTimer _timer;
    QElapsedTimer _elapsed;
    QSize _size;
    double _speed;
    QIcon _icon;
};

#endif // DUWAITICONWIDGET_H
