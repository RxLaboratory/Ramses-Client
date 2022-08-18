#include "shotlistmanagerwidget.h"

#include "ramses.h"
#include "ramsequence.h"

#include "shotscreationdialog.h"
#include "data-models/ramitemtable.h"

ShotListManagerWidget::ShotListManagerWidget(QWidget *parent):
    ObjectListManagerWidget(
        "Shots",
        QIcon(":icons/shot"),
        parent)
{
    changeProject(Ramses::instance()->currentProject());
    connect(Ramses::instance(), SIGNAL(currentProjectChanged(RamProject*)), this, SLOT(changeProject(RamProject*)));
    m_listEditWidget->setEditMode(ObjectListEditWidget::RemoveObjects);
    m_listEditWidget->setSortable(true);

    // Batch create
    QMenu *createMenu = new QMenu(this);

    QAction *createAction = new QAction("Create new shot...");
    createMenu->addAction(createAction);

    QAction *batchAction = new QAction("Create multiple shots...");
    createMenu->addAction(batchAction);

    QToolButton *addButton = m_listEditWidget->addButton();
    addButton->setPopupMode(QToolButton::InstantPopup);
    addButton->setMenu(createMenu);

    connect(createAction, SIGNAL(triggered()), this, SLOT(createObject()));
    connect(batchAction, SIGNAL(triggered()), this, SLOT(batchCreate()));

}

RamShot *ShotListManagerWidget::createObject()
{
    RamProject *project = Ramses::instance()->currentProject();
    if (!project) return nullptr;
    if (project->sequences()->rowCount() == 0 ) return nullptr;

    RamSequence *seq = RamSequence::c( currentFilter() );
    if (!seq) seq = RamSequence::c( project->sequences()->first() );
    if(!seq) return nullptr;

    RamShot *shot = new RamShot(
                "NEW",
                "New Shot",
                seq
                );

    project->shots()->append(shot);
    shot->edit();
    return shot;
}

void ShotListManagerWidget::changeProject(RamProject *project)
{
    // empty list
    this->clear();
    if (!project) return;
    this->setList( project->shots() );
    m_listEditWidget->setFilterList( project->sequences() );
}

void ShotListManagerWidget::batchCreate()
{
    RamProject *project = Ramses::instance()->currentProject();
    if (!project) return;
    if (project->sequences()->rowCount() == 0 ) return;
    RamSequence *seq = RamSequence::get( currentFilterUuid() );
    if (!seq) seq = qobject_cast<RamSequence*>( project->sequences()->at(0) );
    if(!seq) return;

    ShotsCreationDialog dialog(project, this);
    dialog.setSequence(seq);
    dialog.exec();
}
