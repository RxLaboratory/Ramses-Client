#ifndef INSTALLPAGE_H
#define INSTALLPAGE_H

#include "ui_installpage.h"

#include <QWidget>
#include <QDesktopServices>
#include <QUrl>
#include <QSettings>

#include "duqf-app/app-version.h"
#include "duqf-widgets/duqffolderselectorwidget.h"
#include "ramses.h"

class InstallPage : public QWidget, private Ui::InstallPage
{
    Q_OBJECT

public:
    explicit InstallPage(QWidget *parent = nullptr);

signals:
    void login();

private slots:
    void help();
    void login_clicked();
    void setRamsesPath(QString p);
    void serverAddressEdit_editingFinished();
    void sslCheckBox_clicked(bool checked);

private:
    DuQFFolderSelectorWidget *ui_folderSelector;
    QSettings m_settings;

};

#endif // INSTALLPAGE_H
