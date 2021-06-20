#include "assettable.h"

AssetTable::AssetTable(QString title, QWidget *parent):
    ItemTable(title, parent)
{

}

void AssetTable::projectChanged( RamProject *project )
{
    this->setEnabled(false );

    if (!project)
    {
        m_table->clear();
        return;
    }
    else
    {
        //m_table->setList( project->assetGroups(), RamStep::AssetProduction );
    }

    this->setEnabled(true);
}
