#include "shotlistmanagerwidget.h"

ShotListManagerWidget::ShotListManagerWidget(QWidget *parent):
    ObjectListManagerWidget(
        new ShotEditWidget(),
        "Shots",
        QIcon(":icons/shot"),
        parent)
{
    changeProject(Ramses::instance()->currentProject());
    connect(Ramses::instance(), SIGNAL(currentProjectChanged(RamProject*)), this, SLOT(changeProject(RamProject*)));
    m_listEditWidget->setEditMode(ObjectListEditWidget::RemoveObjects);
    m_listEditWidget->setSortable(true);
}

RamObject *ShotListManagerWidget::createObject()
{
    RamProject *project = Ramses::instance()->currentProject();
    if (!project) return nullptr;
    if (project->sequences()->count() == 0 ) return nullptr;
    RamSequence *seq = RamSequence::sequence( currentFilter() );
    if (!seq) seq = qobject_cast<RamSequence*>( project->sequences()->at(0) );
    if(!seq) return nullptr;

    RamShot *shot = new RamShot(
                "NEW",
                seq,
                "New Shot"
                );

    project->shots()->append(shot);
    editObject(shot);
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
