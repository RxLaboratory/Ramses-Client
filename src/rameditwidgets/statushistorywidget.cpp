#include "statushistorywidget.h"

#include "simpleobjectlist.h"

StatusHistoryWidget::StatusHistoryWidget(QWidget *parent) : QWidget(parent)
{
    setupUi();
    setItem(nullptr);
    connectEvents();
    currentProjectChanged(Ramses::instance()->currentProject());
}

StatusHistoryWidget::StatusHistoryWidget(RamItem *item, QWidget *parent) : QWidget(parent)
{
    setupUi();
    setItem(item);
    connectEvents();
    currentProjectChanged(Ramses::instance()->currentProject());
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

    for(int i = 0; i < item->statusHistory()->count(); i++) newStatus(item->statusHistory()->at(i));

    // Set the current status as template for the status editor
    statusWidget->setStatus( _item->status( stepBox->currentData().toString()));

    _itemConnections << connect(item->statusHistory(), &RamObjectList::objectAdded, this, &StatusHistoryWidget::newStatus );
    _itemConnections << connect(item->statusHistory(), &RamObjectList::objectRemoved, this, &StatusHistoryWidget::statusRemoved );

    this->setEnabled(true);
}

void StatusHistoryWidget::newStatus(RamObject *obj)
{
    RamStatus *status = (RamStatus*)obj;
    if (!status->step()) return;
    if (status->step()->uuid() != stepBox->currentData().toString()) return;
    statusList->addObject(status);
    statusList->scrollToBottom();
}

void StatusHistoryWidget::statusRemoved(RamObject *obj)
{
    statusList->removeObject(obj);
}

void StatusHistoryWidget::newStep(RamStep *step)
{
    if(!_item) return;
    if (_item->objectType() == RamObject::Shot && step->type() != RamStep::ShotProduction) return;
    if (_item->objectType() == RamObject::Asset && step->type() != RamStep::AssetProduction) return;

    stepBox->addItem(step->name(), step->uuid());

    connect(step, SIGNAL(removed(RamObject*)), this, SLOT(stepRemoved(RamObject*)));
    connect(step, &RamStep::changed, this, &StatusHistoryWidget::stepChanged);
}

void StatusHistoryWidget::stepChanged(RamObject *step)
{
    for( int row = stepBox->count()-1; row >= 0; row--)
    {
        if (stepBox->itemData(row).toString() == step->uuid()) stepBox->setItemText(row, step->name());
    }
}

void StatusHistoryWidget::stepRemoved(RamObject *step)
{
    stepRemoved(step->uuid());
}

void StatusHistoryWidget::stepRemoved(RamStep *step)
{
    stepRemoved(step->uuid());
}

void StatusHistoryWidget::stepRemoved(QString uuid)
{
    for( int row = stepBox->count()-1; row >= 0; row--)
    {
        if (stepBox->itemData(row).toString() == uuid) stepBox->removeItem(row);
    }
}

void StatusHistoryWidget::currentProjectChanged(RamProject *project)
{
    this->setEnabled(false);
    while( !_projectConnections.isEmpty() ) disconnect( _projectConnections.takeLast() );

    QSignalBlocker b(stepBox);

    QString previousSelection = stepBox->currentData().toString();
    stepBox->clear();

    if (!project) return;

    foreach (RamStep *step, project->steps()) newStep(step);

    for(int i = 0; i < stepBox->count(); i++)
        if (previousSelection == stepBox->itemData(i).toString())
        {
            stepBox->setCurrentIndex(i);
            break;
        }

    _projectConnections << connect(project, &RamProject::newStep, this, &StatusHistoryWidget::newStep );
    _projectConnections << connect(project, SIGNAL(stepRemoved(RamStep*)), this, SLOT(stepRemoved(RamStep*)) );

    this->setEnabled(true);
}

void StatusHistoryWidget::stepFilterChanged(int i)
{
    Q_UNUSED(i);
    setItem(_item);
}

void StatusHistoryWidget::setStatus(RamState *state, int completionRatio, int version, QString comment)
{
    if (!_item) return;

    RamUser *user = Ramses::instance()->currentUser();
    if (!user) return;

    RamProject *project = Ramses::instance()->currentProject();
    if (!project) return;

    RamStep *step = project->step( stepBox->currentData().toString());
    if (!step) return;

    if (!state) return;

    //TODO Check version in files
    _item->setStatus(user, state, step, completionRatio, comment, version);
}

void StatusHistoryWidget::setupUi()
{
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(3);

    QHBoxLayout *sLayout = new QHBoxLayout();
    sLayout->setSpacing(3);
    sLayout->setContentsMargins(0,0,0,0);

    QLabel *statusLabel = new QLabel("Status", this);
    sLayout->addWidget(statusLabel);

    stepBox = new QComboBox(this);
    sLayout->addWidget(stepBox);

    sLayout->setStretch(0,0);
    sLayout->setStretch(1,100);

    layout->addLayout(sLayout);

    QSplitter *splitter = new QSplitter(this);
    splitter->setOrientation(Qt::Vertical);

    statusList = new SimpleObjectList(true, splitter);
    statusList->setEditable(false);
    splitter->addWidget( statusList );

    statusWidget = new StatusEditWidget(nullptr, splitter);
    statusWidget->hideStatus();
    splitter->addWidget(statusWidget);

    layout->addWidget(splitter);

    splitter->setSizes(QList<int>() << 75 << 25);

    this->setLayout(layout);
}

void StatusHistoryWidget::connectEvents()
{
    connect(Ramses::instance(), &Ramses::currentProjectChanged, this, &StatusHistoryWidget::currentProjectChanged);
    connect(stepBox, SIGNAL(currentIndexChanged(int)), this, SLOT(stepFilterChanged(int)));
    connect(statusWidget, &StatusEditWidget::statusUpdated, this, &StatusHistoryWidget::setStatus);
}
