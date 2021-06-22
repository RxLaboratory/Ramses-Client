#ifndef PIPEEDITWIDGET_H
#define PIPEEDITWIDGET_H

#include <QComboBox>
#include <QMenu>

#include "objecteditwidget.h"
#include "objectlisteditwidget.h"
#include "ramses.h"

/**
 * @brief The PipeEditWidget class is used to edit a RamPipe and is usually shown in the MainWindow Dock
 */
class PipeEditWidget : public ObjectEditWidget
{
public:
    PipeEditWidget(RamPipe *pipe = nullptr, QWidget *parent = nullptr);

public slots:
    void setObject(RamObject *pipeObj) override;

protected slots:
    void update() override;

private slots:
    void appChanged();
    // PipeFiles
    void assignPipeFile();
    void pipeFileAssigned(RamObject *pipeFileObj);
    void pipeFileUnassigned(RamObject *pipeFileObj);
    void newPipeFile(RamObject *pipeFileObj);
    void pipeFileChanged(RamObject *pipeFileObj);
    void pipeFileRemoved(RamObject *pipeFileObj);
    void createPipeFile();


private:
    RamPipe *_pipe = nullptr;

    void setupUi();
    void connectEvents();
    void populateMenus(RamProject *project);

    QComboBox *m_fromBox;
    QComboBox *m_toBox;
    ObjectListEditWidget *m_pipeFileList;
    QMenu *m_assignPipeFileMenu;
    QAction *m_newPipeFileAction;

    QMap<QString, QList<QMetaObject::Connection>> m_pipeFileConnections;
    QList<QMetaObject::Connection> m_projectConnections;
};

#endif // PIPEEDITWIDGET_H
