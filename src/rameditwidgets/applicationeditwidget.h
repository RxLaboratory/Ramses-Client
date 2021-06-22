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
    void setObject(RamObject *obj) override;

protected slots:
    void update() override;

private slots:
    void createForNative();
    void createForImport();
    void createForExport();

private:
    RamApplication *m_application;

    void setupUi();
    void connectEvents();

    DuQFFolderSelectorWidget *m_folderSelector;
    ObjectListEditWidget *m_nativeList;
    ObjectListEditWidget *m_importList;
    ObjectListEditWidget *m_exportList;
};

#endif // APPLICATIONEDITWIDGET_H
