#ifndef DUICONWIDGET_H
#define DUICONWIDGET_H

#include <QIcon>
#include <QTimer>
#include <QWidget>
#include <QElapsedTimer>


class DuIconWidget : public QWidget
{
    Q_OBJECT
public:
    enum Animation {
        None,
        RotateCW,
        RotateCCW,
    };

    explicit DuIconWidget(QWidget *parent = nullptr);

    // Settings
    void setSize(const QSize &s);;
    void setSVGIcon(const QString &iconPath);

    // Animation
    void rotate(double speed = 1.0, double fps = 24.0, bool ccw = false);
    void stopAnimation();

protected:
    virtual void showEvent(QShowEvent *) override;;
    virtual void hideEvent(QHideEvent *) override;;
    virtual void paintEvent(QPaintEvent *e) override;

private:
    QTimer _timer;
    QElapsedTimer _elapsed;
    QSize _size = QSize(16,16);
    double _speed = 1.0;
    QIcon _icon;
    QString _iconPath;
    QPixmap _pix;
    Animation _animation = None;
};

#endif // DUICONWIDGET_H
