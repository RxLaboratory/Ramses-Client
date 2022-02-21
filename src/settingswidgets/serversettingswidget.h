#ifndef SERVERSETTINGSWIDGET_H
#define SERVERSETTINGSWIDGET_H

#include "ramses.h"
#include "duqf-app/app-utils.h"
#include "duqf-widgets/duqfservercombobox.h"
#include "duqf-widgets/duqfsslcheckbox.h"

#include <QSettings>

class ServerSettingsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ServerSettingsWidget(QWidget *parent = nullptr);
private slots:
    void updateFreqSpinBox_editingFinished();
    void timeoutSpinBox_editingFinished();
    void logout();
    void dbiConnectionStatusChanged(NetworkUtils::NetworkStatus s);
private:
    void setupUi();
    void connectEvents();

    QPushButton *ui_logoutButton;
    DuQFServerComboBox *ui_serverAddressBox;
    DuQFSSLCheckbox *ui_sslCheckBox;
    QSpinBox *ui_updateFreqSpinBox;
    QSpinBox *ui_timeoutSpinBox;
    QWidget *ui_logoutWidget;
    QWidget *ui_settingsWidget;

    QSettings m_settings;
    DuApplication *_app;
};

#endif // SERVERSETTINGSWIDGET_H
