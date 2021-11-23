#ifndef SERVERSETTINGSWIDGET_H
#define SERVERSETTINGSWIDGET_H

#include "ui_serversettingswidget.h"

#include "ramses.h"
#include "duqf-app/app-utils.h"

#include <QSettings>

class ServerSettingsWidget : public QWidget, private Ui::ServerSettingsWidget
{
    Q_OBJECT

public:
    explicit ServerSettingsWidget(QWidget *parent = nullptr);
private slots:
    void serverAddressEdit_editingFinished(QString address);
    void sslCheckBox_clicked(bool checked);
    void updateFreqSpinBox_editingFinished();
    void timeoutSpinBox_editingFinished();
    void logout();
    void dbiConnectionStatusChanged(NetworkUtils::NetworkStatus s);
private:
    QSettings m_settings;
    DuApplication *_app;
};

#endif // SERVERSETTINGSWIDGET_H
