#ifndef LANDINGPAGE_H
#define LANDINGPAGE_H

#include <QWidget>
#include <QPushButton>
#include <QToolButton>
#include <QStackedLayout>

#include "duwidgets/ducombobox.h"

class LandingPage : public QWidget
{
    Q_OBJECT
public:
    explicit LandingPage(QWidget *parent = nullptr);

private slots:
    void updateRecentList();
    void createLocalProject();

private:
    void setupUi();
    void connectEvents();

    QStackedLayout *ui_stackedLayout;
    QPushButton *ui_createLocalProjectButton;
    QPushButton *ui_createTeamProjectButton;
    QPushButton *ui_openProjectButton;
    QPushButton *ui_joinTeamProjectButton;
    QToolButton *ui_openRecentProjectButton;
    DuComboBox *ui_recentBox;
};

#endif // LANDINGPAGE_H
