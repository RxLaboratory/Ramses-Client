#ifndef PROJECTEDITWIDGET_H
#define PROJECTEDITWIDGET_H

#include "ui_projecteditwidget.h"
#include "ramses.h"
#include "duqf-widgets/duqffolderselectorwidget.h"

class ProjectEditWidget : public QWidget, private Ui::ProjectEditWidget
{
    Q_OBJECT

public:
    explicit ProjectEditWidget(QWidget *parent = nullptr);

    RamProject *project() const;
    void setProject(RamProject *project);

private slots:
    void update();
    void revert();
    bool checkInput();
    void updateFolderLabel(QString path);
    void projectRemoved(RamObject *o);
    void dbiLog(DuQFLog m);

private:
    DuQFFolderSelectorWidget *folderSelector;
    RamProject *_project;
    QMetaObject::Connection _currentProjectConnection;
};

#endif // PROJECTEDITWIDGET_H
