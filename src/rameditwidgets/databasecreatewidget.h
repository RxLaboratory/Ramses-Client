#ifndef DATABASECREATEWIDGET_H
#define DATABASECREATEWIDGET_H


#include "duqf-widgets/duqffolderselectorwidget.h"
#include "localdatainterface.h"
#include "servereditwidget.h"
#include "duqf-widgets/duscrollarea.h"


class DatabaseCreateWidget : public DuScrollArea
{
    Q_OBJECT
public:
    DatabaseCreateWidget(QWidget *parent = nullptr);

private slots:
    void checkPath(QString p);
    void createDB();
    void finishSync();

private:
    void setupUi();
    void connectEvents();

    bool createNewDB();
    bool createNewDB(ServerConfig s);

    void createOnlineDB();
    void createOfflineDB();

    DuQFFolderSelectorWidget *ui_fileSelector;
    DuQFFolderSelectorWidget *ui_folderSelector;
    QTabWidget *ui_tabWidget;
    QLineEdit *ui_shortNameEdit;
    QLineEdit *ui_nameEdit;
    ServerEditWidget *ui_serverEdit;
    QPushButton *ui_createButton;

    bool m_downloading = false;
};

#endif // DATABASECREATEWIDGET_H
