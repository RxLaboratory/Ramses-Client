#ifndef APPLICATIONEDITWIDGET_H
#define APPLICATIONEDITWIDGET_H

#include <QMenu>
#include <QSplitter>

#include "objecteditwidget.h"
#include "duqf-widgets/duqffolderselectorwidget.h"
#include "ramapplication.h"

class ObjectListEditWidget;

class ApplicationEditWidget : public ObjectEditWidget
{
    Q_OBJECT

public:
    explicit ApplicationEditWidget(QWidget *parent = nullptr);
    explicit ApplicationEditWidget(RamApplication *app, QWidget *parent = nullptr);

    RamApplication *application() const;

public slots:
    void setObject(RamObject *obj) Q_DECL_OVERRIDE;

protected slots:
    void update() Q_DECL_OVERRIDE;

private slots:
    void newFileType(RamObject * const ft);
    void fileTypeChanged(RamObject *o);
    void fileTypeRemoved(RamObject *o);
    void assignNativeFileType();
    void assignImportFileType();
    void assignExportFileType();

    void nativeFileTypeAssigned(RamObject * const ft);
    void importFileTypeAssigned(RamObject * const ft);
    void exportFileTypeAssigned(RamObject * const ft);
    void nativeFileTypeUnassigned(RamObject * const ft);
    void importFileTypeUnassigned(RamObject * const ft);
    void exportFileTypeUnassigned(RamObject * const ft);

private:
    RamApplication *_application;

    void setupUi();
    void populateMenus();
    void connectEvents();

    DuQFFolderSelectorWidget *folderSelector;
    ObjectListEditWidget *nativeList;
    ObjectListEditWidget *importList;
    ObjectListEditWidget *exportList;

    QMenu *nativeMenu;
    QMenu *importMenu;
    QMenu *exportMenu;

    QMap<QString, QList<QMetaObject::Connection>> m_fileTypeConnections;
};

#endif // APPLICATIONEDITWIDGET_H
