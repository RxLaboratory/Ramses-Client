#ifndef DATABASECREATEWIDGET_H
#define DATABASECREATEWIDGET_H

#include <QScrollArea>

#include "duqf-widgets/duqffolderselectorwidget.h"

class DatabaseCreateWidget : public QScrollArea
{
    Q_OBJECT
public:
    DatabaseCreateWidget(QWidget *parent = nullptr);

private slots:
    void checkPath(QString p);
    void createDB();

private:
    void setupUi();
    void connectEvents();

    DuQFFolderSelectorWidget *ui_fileSelector;
    QTabWidget *ui_tabWidget;
    QLineEdit *ui_shortNameEdit;
    QLineEdit *ui_npassword1Edit;
    QLineEdit *ui_npassword2Edit;
    QLineEdit *ui_serverAddressEdit;
    QCheckBox *ui_sslCheckBox;
    QSpinBox *ui_updateFreqSpinBox;
    QSpinBox *ui_timeoutSpinBox;
    QPushButton *ui_createButton;
};

#endif // DATABASECREATEWIDGET_H
