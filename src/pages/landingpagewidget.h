#ifndef LANDINGPAGEWIDGET_H
#define LANDINGPAGEWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QToolButton>
#include <QStackedLayout>
#include <QWizard>

#include "datastruct.h"
#include "duwidgets/ducombobox.h"

class LandingPageWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LandingPageWidget(QWidget *parent = nullptr);

private slots:
    void updateRecentList();
    void createDatabase(bool team = false);
    void openDatabase(const QString &dbFile);
    void joinTeamProject();
    void manageServer();

private:
    void setupUi();
    void connectEvents();

    QStackedLayout *ui_stackedLayout;
    QPushButton *ui_createLocalProjectButton;
    QPushButton *ui_createTeamProjectButton;
    QPushButton *ui_openDatabaseButton;
    QPushButton *ui_joinTeamProjectButton;
    QToolButton *ui_openRecentDatabaseButton;
    QToolButton *ui_manageServerButton;
    QToolButton *ui_helpButton;
    QToolButton *ui_donateButton;
    DuComboBox *ui_recentBox;

    QString login(ServerConfig serverSettings, const QString &username, const QString &password, bool silentFail = false);
    void showWizard(QWizard *w);
};

#endif // LANDINGPAGEWIDGET_H
