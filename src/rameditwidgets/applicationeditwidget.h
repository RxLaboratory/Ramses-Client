#ifndef APPLICATIONEDITWIDGET_H
#define APPLICATIONEDITWIDGET_H

#include <QMenu>
#include <QSplitter>

#include "objecteditwidget.h"
#include "duqf-widgets/duqffolderselectorwidget.h"
#include "simpleobjectlist.h"

class ApplicationEditWidget : public ObjectEditWidget
{
    Q_OBJECT

public:
    explicit ApplicationEditWidget(QWidget *parent = nullptr);
    explicit ApplicationEditWidget(RamApplication *app, QWidget *parent = nullptr);

    RamApplication *application() const;
    void setApplication(RamApplication *application);

protected slots:
    void update() Q_DECL_OVERRIDE;

private slots:
    void applicationChanged(RamObject *o);
    void newFileType(RamObject * const ft);
    void fileTypeChanged(RamObject *o);
    void fileTypeRemoved(RamObject *o);
    void assignNativeFileType();
    void assignImportFileType();
    void assignExportFileType();
    void unassignNativeFileType(RamObject *o);
    void unassignImportFileType(RamObject *o);
    void unassignExportFileType(RamObject *o);
    void nativeFileTypeAssigned(RamFileType * const ft);
    void importFileTypeAssigned(RamFileType * const ft);
    void exportFileTypeAssigned(RamFileType * const ft);
    void nativeFileTypeUnassigned(QString uuid);
    void importFileTypeUnassigned(QString uuid);
    void exportFileTypeUnassigned(QString uuid);

private:
    RamApplication *_application;

    void setupUi();
    void populateMenus();
    void connectEvents();

    DuQFFolderSelectorWidget *folderSelector;
    SimpleObjectList *nativeList;
    SimpleObjectList *importList;
    SimpleObjectList *exportList;

    QMenu *nativeMenu;
    QMenu *importMenu;
    QMenu *exportMenu;
};

#endif // APPLICATIONEDITWIDGET_H
