#include "shotmanagerwidget.h"

#include "ramses.h"
#include "ramsequence.h"

#include "shotscreationdialog.h"
#include "duwidgets/duicon.h"
#include "duapp/duui.h"

ShotManagerWidget::ShotManagerWidget(QWidget *parent):
    ObjectManagerWidget(
        "Shots",
        DuIcon(":icons/shot"),
        parent)
{
    setProject(Ramses::i()->project());
    ui_listWidget->setEditMode(ObjectListWidget::RemoveObjects);
    ui_listWidget->setSortable(true);

    // Batch create
    DuMenu *createMenu = new DuMenu(this);

    QAction *createAction = new QAction("Create new shot...");
    createMenu->addAction(createAction);

    QAction *batchAction = new QAction("Create multiple shots...");
    createMenu->addAction(batchAction);

    QToolButton *addButton = ui_listWidget->addButton();
    addButton->setPopupMode(QToolButton::InstantPopup);
    addButton->setMenu(createMenu);

    connect(createAction, SIGNAL(triggered()), this, SLOT(createObject()));
    connect(batchAction, SIGNAL(triggered()), this, SLOT(batchCreate()));

}

RamShot *ShotManagerWidget::createObject()
{
    RamProject *project = Ramses::i()->project();
    if (!project) return nullptr;
    if (project->sequences()->rowCount() == 0 ) return nullptr;

    RamSequence *seq = RamSequence::c( currentFilter() );
    if (!seq) seq = RamSequence::c( project->sequences()->get(0) );
    if(!seq) return nullptr;

    RamShot *shot = new RamShot(
                "NEW",
                "New Shot",
                seq
                );

    //project->shots()->appendObject(shot->uuid());
    shot->edit();
    return shot;
}

void ShotManagerWidget::setProject(RamProject *project)
{
    // empty list
    this->clear();
    if (!project) return;
    this->setObjectModel( project->shots() );
    ui_listWidget->setFilterList( project->sequences(), "Shots" );
}

void ShotManagerWidget::batchCreate()
{
    RamProject *project = Ramses::i()->project();
    if (!project) return;
    if (project->sequences()->rowCount() == 0 ) return;
    RamSequence *seq = RamSequence::c( currentFilter() );
    if (!seq) seq = RamSequence::c( project->sequences()->get(0) );
    if(!seq) return;

    ShotsCreationDialog dialog(project, this);
    dialog.setSequence(seq);
    dialog.exec();
}
