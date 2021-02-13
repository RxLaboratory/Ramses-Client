#ifndef SERVERSETTINGSWIDGET_H
#define SERVERSETTINGSWIDGET_H

#include "ui_serversettingswidget.h"

#include "ramses.h"

#include <QSettings>

class ServerSettingsWidget : public QWidget, private Ui::ServerSettingsWidget
{
    Q_OBJECT

public:
    explicit ServerSettingsWidget(QWidget *parent = nullptr);
private slots:
    void serverAddressEdit_edingFinished();
    void sslCheckBox_clicked(bool checked);
    void logout();
    void dbiStatusChanged(NetworkUtils::NetworkStatus s, QString m);
private:
    QSettings settings;
};

#endif // SERVERSETTINGSWIDGET_H
