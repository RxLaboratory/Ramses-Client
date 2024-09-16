#ifndef HOMEPAGE_H
#define HOMEPAGE_H

#include "duwidgets/duwaiticonwidget.h"
#include <QWidget>
#include <QProgressBar>
#include <QLabel>

class HomePage : public QWidget
{
    Q_OBJECT
public:
    explicit HomePage(QWidget *parent = nullptr);

signals:

private slots:
    void setWaiting();
    void setWorking();

private:
    void setupUi();
    void connectEvents();

    QLabel *ui_progressTitleLabel;
    QLabel *ui_progressTextLabel;
    DuWaitIconWidget *ui_waitIcon;
};

#endif // HOMEPAGE_H
