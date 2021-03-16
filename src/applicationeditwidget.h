#ifndef APPLICATIONEDITWIDGET_H
#define APPLICATIONEDITWIDGET_H

#include "ui_applicationeditwidget.h"
#include "duqf-widgets/duqffolderselectorwidget.h"
#include "ramses.h"

class ApplicationEditWidget : public QWidget, private Ui::ApplicationEditWidget
{
    Q_OBJECT

public:
    explicit ApplicationEditWidget(QWidget *parent = nullptr);

    RamApplication *application() const;
    void setApplication(RamApplication *application);

private slots:
    void update();
    void revert();
    bool checkInput();
    void applicationRemoved(RamObject *o);
    void dbiLog(DuQFLog m);

private:
    DuQFFolderSelectorWidget *folderSelector;
    RamApplication *_application;
    QMetaObject::Connection _currentApplicationConnection;
};

#endif // APPLICATIONEDITWIDGET_H
