#include "shottable.h"

ShotTable::ShotTable(QString title, QWidget *parent):
    ItemTable(title, parent)
{

}

void ShotTable::projectChanged( RamProject *project )
{
    this->setEnabled(false );

    if (!project)
    {
        m_table->clear();
        return;
    }
    else
    {
        m_table->setList( project->sequences(), project->steps(), RamStep::ShotProduction );
    }

    this->setEnabled(true);
}
