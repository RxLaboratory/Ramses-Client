#ifndef APPLICATIONEDITWIDGET_H
#define APPLICATIONEDITWIDGET_H

#include <QMenu>

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
    void newFileType(RamFileType * const ft);
    void fileTypeChanged();
    void fileTypeRemoved(RamObject *o);
    void assignNativeFileType();
    void assignImportFileType();
    void assignExportFileType();
    void unassignNativeFileType();
    void unassignImportFileType();
    void unassignExportFileType();
    void nativeFileTypeAssigned(RamFileType * const ft);
    void importFileTypeAssigned(RamFileType * const ft);
    void exportFileTypeAssigned(RamFileType * const ft);
    void nativeFileTypeUnassigned(QString uuid);
    void importFileTypeUnassigned(QString uuid);
    void exportFileTypeUnassigned(QString uuid);
    void dbiLog(DuQFLog m);

private:
    DuQFFolderSelectorWidget *folderSelector;
    RamApplication *_application;
    QMenu *nativeMenu;
    QMenu *importMenu;
    QMenu *exportMenu;
    QList<QMetaObject::Connection> _applicationConnections;

};

#endif // APPLICATIONEDITWIDGET_H
