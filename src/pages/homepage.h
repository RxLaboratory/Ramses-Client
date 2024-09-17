#ifndef HOMEPAGE_H
#define HOMEPAGE_H

#include <QWidget>
#include <QProgressBar>
#include <QLabel>

#include "duwidgets/duiconwidget.h"
#include "statemanager.h"

class HomePage : public QWidget
{
    Q_OBJECT
public:
    explicit HomePage(QWidget *parent = nullptr);

signals:

private slots:
    void changeState(StateManager::State s);

private:
    void setupUi();
    void connectEvents();

    QLabel *ui_progressTitleLabel;
    QLabel *ui_progressTextLabel;
    DuIconWidget *ui_waitIcon;
};

#endif // HOMEPAGE_H
