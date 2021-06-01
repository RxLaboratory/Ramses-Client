#include "statushistorywidget.h"

#include "simpleobjectlist.h"

StatusHistoryWidget::StatusHistoryWidget(QWidget *parent) : QWidget(parent)
{
    setupUi();
    setItem(nullptr);
    connectEvents();
}

StatusHistoryWidget::StatusHistoryWidget(RamItem *item, QWidget *parent) : QWidget(parent)
{
    setupUi();
    setItem(item);
    connectEvents();
}

void StatusHistoryWidget::setItem(RamItem *item)
{
    this->setEnabled(false);

    while( !_itemConnections.isEmpty() ) disconnect( _itemConnections.takeLast() );

    _item = item;

    QSignalBlocker b(statusList);

    statusList->clear();
    currentProjectChanged(Ramses::instance()->currentProject());

    if (!_item) return;

    statusList->setList( item->statusHistory(), false );
    currentProjectChanged( Ramses::instance()->currentProject() );

    // Set the current status as template for the status editor
    statusWidget->setStatus( item->status( statusList->currentFilter() ));

    this->setEnabled(true);
}

void StatusHistoryWidget::newStep(RamObject *obj)
{
    if(!_item) return;
    RamStep *step = qobject_cast<RamStep*>( obj );
    if (_item->objectType() == RamObject::Shot && step->type() != RamStep::ShotProduction) return;
    if (_item->objectType() == RamObject::Asset && step->type() != RamStep::AssetProduction) return;

    statusList->addFilter( step );
}

void StatusHistoryWidget::stepRemoved(RamObject *step)
{
    statusList->removeFilter( step );
}

void StatusHistoryWidget::currentProjectChanged(RamProject *project)
{
    this->setEnabled(false);
    while( !_projectConnections.isEmpty() ) disconnect( _projectConnections.takeLast() );

    if (!project) return;

    for (int i = 0; i < project->steps()->count(); i++) newStep( project->steps()->at(i) );

    _projectConnections << connect(project->steps(), &RamObjectList::objectAdded, this, &StatusHistoryWidget::newStep );
    _projectConnections << connect(project->steps(), &RamObjectList::objectRemoved, this, &StatusHistoryWidget::stepRemoved );

    this->setEnabled(true);
}

void StatusHistoryWidget::setStatus(RamState *state, int completionRatio, int version, QString comment)
{
    if (!_item) return;

    RamUser *user = Ramses::instance()->currentUser();
    if (!user) return;

    RamProject *project = Ramses::instance()->currentProject();
    if (!project) return;

    RamStep *step = qobject_cast<RamStep*>( project->steps()->fromUuid( statusList->currentFilter() ) );
    if (!step) return;

    if (!state) return;

    //TODO Check version in files
    _item->setStatus(user, state, step, completionRatio, comment, version);
}

void StatusHistoryWidget::currentFilterChanged(QString f)
{
    statusWidget->setEnabled( f != "");
}

void StatusHistoryWidget::setupUi()
{
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(3);

    QSplitter *splitter = new QSplitter(this);
    splitter->setOrientation(Qt::Vertical);

    statusList = new ObjectListEditWidget(true, splitter);
    statusList->setTitle("Status");
    statusList->setEditable(false);
    statusList->setSearchable(false);
    splitter->addWidget( statusList );

    statusWidget = new StatusEditWidget(nullptr, splitter);
    statusWidget->hideStatus();
    statusWidget->setEnabled(false);
    splitter->addWidget(statusWidget);

    layout->addWidget(splitter);

    splitter->setSizes(QList<int>() << 75 << 25);

    this->setLayout(layout);
}

void StatusHistoryWidget::connectEvents()
{
    connect(Ramses::instance(), &Ramses::currentProjectChanged, this, &StatusHistoryWidget::currentProjectChanged);
    connect(statusWidget, &StatusEditWidget::statusUpdated, this, &StatusHistoryWidget::setStatus);
    connect(statusList, SIGNAL(currentFilterChanged(QString)), this, SLOT(currentFilterChanged(QString)));
}
