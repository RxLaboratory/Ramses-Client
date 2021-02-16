#ifndef PROJECTEDITWIDGET_H
#define PROJECTEDITWIDGET_H

#include "ui_projecteditwidget.h"
#include "ramses.h"

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
    void projectDestroyed(QObject *o);
    void dbiLog(QString m, LogUtils::LogType t);

private:
    RamProject *_project;
    QMetaObject::Connection _currentProjectConnection;
};

#endif // PROJECTEDITWIDGET_H
