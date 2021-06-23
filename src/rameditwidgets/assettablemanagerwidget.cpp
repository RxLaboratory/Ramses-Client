#include "assettablemanagerwidget.h"

AssetTableManagerWidget::AssetTableManagerWidget(QString title, QWidget *parent):
    ItemTableManagerWidget(title, parent)
{

}

void AssetTableManagerWidget::projectChanged( RamProject *project )
{
    this->setEnabled(false );

    if (!project)
    {
        ui_table->setList(nullptr);
        return;
    }
    else
    {
        ui_table->setList( project->assets() );
    }

    this->setEnabled(true);
}
