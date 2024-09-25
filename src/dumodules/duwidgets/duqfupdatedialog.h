#ifndef DUQFUPDATEDIALOG_H
#define DUQFUPDATEDIALOG_H

#include <QJsonObject>
#include <QLabel>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QPushButton>
#include <QStringBuilder>
#include <QDesktopServices>

#include "duwidgets/dudialog.h"

class DuQFUpdateDialog : public DuDialog
{
    Q_OBJECT
public:
    explicit DuQFUpdateDialog(QJsonObject updateInfo, QWidget *parent = nullptr);

signals:

private slots:
    void download();
    void changelog();
    void donate();

private:
    void setupUi(QJsonObject updateInfo);

    QPushButton *ui_downloadButton;
    QPushButton *ui_changelogButton;
    QPushButton *ui_donateButton;
    QPushButton *ui_okButton;

    QString m_downloadURL = "";
    QString m_changelogURL = "";
    QString m_donateURL = "";
};

#endif // DUQFUPDATEDIALOG_H
