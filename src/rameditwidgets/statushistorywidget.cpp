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

    if (!_item) return;

    statusList->setList( item->statusHistory() );

    // Set the current status as template for the status editor
    statusWidget->setStatus( item->status( statusList->currentFilterUuid() ));

    this->setEnabled(true);
}

void StatusHistoryWidget::setStatus(RamState *state, int completionRatio, int version, QString comment)
{
    if (!_item) return;

    RamUser *user = Ramses::instance()->currentUser();
    if (!user) return;

    RamProject *project = Ramses::instance()->currentProject();
    if (!project) return;

    RamStepStatusHistory *history = qobject_cast<RamStepStatusHistory*>( statusList->currentFilter() );
    RamStep *step = history->step();
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
    statusList->setContainingType(RamObject::Status);
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
    connect(statusWidget, &StatusEditWidget::statusUpdated, this, &StatusHistoryWidget::setStatus);
    connect(statusList, SIGNAL(currentFilterChanged(QString)), this, SLOT(currentFilterChanged(QString)));
}
