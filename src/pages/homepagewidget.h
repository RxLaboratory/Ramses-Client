#ifndef HOMEPAGEWIDGET_H
#define HOMEPAGEWIDGET_H

#include <QWidget>
#include <QProgressBar>
#include <QLabel>

#include "duwidgets/duiconwidget.h"
#include "statemanager.h"

class HomePageWidget : public QWidget
{
    Q_OBJECT
public:
    explicit HomePageWidget(QWidget *parent = nullptr);

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

#endif // HOMEPAGEWIDGET_H
