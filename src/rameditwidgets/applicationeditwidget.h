#ifndef APPLICATIONEDITWIDGET_H
#define APPLICATIONEDITWIDGET_H

#include <QMenu>
#include <QSplitter>

#include "objecteditwidget.h"
#include "duqf-widgets/duqffolderselectorwidget.h"
#include "ramses.h"

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
    void newFileType(const QModelIndex &parent,int first,int last);
    void newFileType(RamObject *fileTypeObj);
    void fileTypeChanged(const QModelIndex &first, const QModelIndex &last);
    void fileTypeRemoved(const QModelIndex &parent,int first,int last);
    void assignNativeFileType();
    void assignImportFileType();
    void assignExportFileType();

    void nativeFileTypeAssigned(const QModelIndex &parent,int first,int last);
    void importFileTypeAssigned(const QModelIndex &parent,int first,int last);
    void exportFileTypeAssigned(const QModelIndex &parent,int first,int last);
    void nativeFileTypeUnassigned(const QModelIndex &parent,int first,int last);
    void importFileTypeUnassigned(const QModelIndex &parent,int first,int last);
    void exportFileTypeUnassigned(const QModelIndex &parent,int first,int last);

private:
    RamApplication *m_application;

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
