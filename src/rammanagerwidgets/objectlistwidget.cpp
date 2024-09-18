#include "objectlistwidget.h"

#include <QDesktopServices>

#include "duapp/app-version.h"

#include "duwidgets/duicon.h"
#include "ramses.h"
#include "ramobjectmodel.h"
#include "duapp/duui.h"

ObjectListWidget::ObjectListWidget(bool editableObjects, RamUser::UserRole editRole, QWidget *parent) :
    QWidget(parent)
{
    setupUi(editableObjects, editRole);
    connectEvents();
}

ObjectListWidget::ObjectListWidget(QAbstractItemModel *objectList, bool editableObjects, RamUser::UserRole editRole, QWidget *parent) :
    QWidget(parent)
{
    setupUi(editableObjects, editRole);
    connectEvents();
    setObjectModel(objectList);
}

void ObjectListWidget::setObjectModel(QAbstractItemModel *objectModel)
{
    if (ui_objectView->model())
    {
        disconnect(ui_objectView->model(), nullptr, this, nullptr);
    }
    // Show all
    ui_assignMenu->showAll();

    setFilterList(nullptr);
    ui_objectView->setObjectModel(objectModel);

    if (!objectModel) return;

    connect(ui_objectView->model(), SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(assignObject(QModelIndex,int,int)));
    connect(ui_objectView->model(), SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)), this, SLOT(unassignObject(QModelIndex,int,int)));

    assignObject(QModelIndex(), 0, objectModel->rowCount() - 1);
}

void ObjectListWidget::setFilterList(QAbstractItemModel *filterList, QString filterListName)
{
    ui_filterBox->setObjectModel(filterList, filterListName);
    if (filterList)
    {
        ui_title->hide();
        ui_filterBox->show();
    }
    else
    {
        ui_title->show();
        ui_filterBox->hide();
    }
}

void ObjectListWidget::setAssignList(QAbstractItemModel *assignList)
{
    ui_assignMenu->setObjectModel(assignList);
    ui_addButton->setPopupMode(QToolButton::InstantPopup);
    ui_addButton->setMenu(ui_assignMenu);

    // hide already assigned
    /*if(m_objectModel)
    {
        if (m_objectModel->rowCount())
            objectAssigned(QModelIndex(), 0, m_objectModel->rowCount() - 1);
    }*/

    m_useAssignList = true;
}

void ObjectListWidget::setDontRemoveShortNameList(QStringList dontRemove)
{
    m_dontRemove = dontRemove;
}

void ObjectListWidget::clear()
{
    ui_objectView->setModel(nullptr);
}

void ObjectListWidget::setEditMode(EditMode editMode)
{
    m_editMode = editMode;
    switch(editMode) {
    case NoEdit:
        ui_addButton->hide();
        ui_removeButton->hide();
        break;
    case UnassignObjects:
    case RemoveObjects:
        ui_addButton->show();
        ui_removeButton->show();
        break;
    }
}

void ObjectListWidget::setEditable(bool editable)
{
    ui_removeButton->setVisible(editable);
    ui_addButton->setVisible(editable);
}

void ObjectListWidget::setSearchable(bool searchable)
{
    ui_searchEdit->setVisible(searchable);
}

void ObjectListWidget::setSortable(bool sortable)
{
    ui_objectView->setSortable(sortable);
}

void ObjectListWidget::setTitle(QString title)
{
    ui_title->setVisible(title != "");
    ui_title->setText(title);
}

void ObjectListWidget::select(RamObject *o)
{
    ui_objectView->select(o);
}

void ObjectListWidget::setFilter(RamObject *o)
{
    QSignalBlocker b(ui_filterBox);
    ui_filterBox->setObject(o);
    ui_assignMenu->setFilterObject(o);
    ui_objectView->filter(o);
}

QToolButton *ObjectListWidget::addButton() const
{
    return ui_addButton;
}

QString ObjectListWidget::currentFilterUuid() const
{
    return ui_filterBox->currentUuid();
}

RamObject *ObjectListWidget::currentFilter() const
{
    return ui_filterBox->currentObject();
}

void ObjectListWidget::setSortMode(RamObject::DataRole mode)
{
    ui_objectView->setSortRole(mode);
}

void ObjectListWidget::sort()
{
    ui_objectView->sort();
}

RamObjectView *ObjectListWidget::listWidget()
{
    return ui_objectView;
}

void ObjectListWidget::removeSelectedObjects()
{
    if (m_editMode == NoEdit)
        return;

    QModelIndexList selection = ui_objectView->selectionModel()->selectedRows();
    if (selection.count() == 0) return;

    if (m_editMode == RemoveObjects)
    {
        QMessageBox::StandardButton confirm = QMessageBox::question( this,
            "Confirm deletion",
            "Are you sure you want to premanently remove the selected items?" );

        if ( confirm != QMessageBox::Yes) return;
    }

    // Check if we can remove these objects
    for (int i = 0; i < selection.count(); i++)
    {
        quintptr iptr = selection.at(i).data(RamObject::Pointer).toULongLong();
        if (iptr == 0) continue;
        RamObject *o = reinterpret_cast<RamObject*>(iptr);

        // Don't remove yourself if you're a user
        if (o->objectType() == RamObject::User && m_editMode == RemoveObjects)
        {
            if (o->is( Ramses::i()->currentUser() ))
            {
                QMessageBox::information(this,
                                         "Can't remove this user",
                                         "Sorry, you can't remove yourself!\nAsk for someone else to remove you, that's safer.");
                return;
            }
        }
        if (m_dontRemove.contains(o->shortName()))
        {
            QMessageBox::information(this,
                                     "Can't remove this",
                                     "Sorry, " + o->shortName() + " | " + o->name() + " must not be removed.");
            return;
        }
        if (o->shortName() == "Duduf" && o->comment() != "")
        {
            QMessageBox::StandardButton result = QMessageBox::question(this,
                                     "Please read this before removing Duduf!",
                                     o->comment() + "\n\nDo you want to go to the website to make a donation?");
            if (result == QMessageBox::Yes)
            {
                QDesktopServices::openUrl(QUrl(URL_DONATION));
                return;
            }
        }
    }

    QVector <RamObject*> removedObjs;

    for( int i = selection.count() -1; i >= 0; i--)
    {
        QString uuid = selection.at(i).data(RamObject::UUID).toString();
        quintptr iptr = selection.at(i).data(RamObject::Pointer).toULongLong();
        if (iptr == 0) continue;
        RamObject *o = reinterpret_cast<RamObject*>(iptr);

        if (m_editMode == RemoveObjects) if (o) o->remove();

        // If this is an object model we need to remove the object from it
        RamObjectModel *objList = qobject_cast<RamObjectModel*>( ui_objectView->objectModel() );
        if (objList) objList->removeObjects(QStringList(uuid));

        removedObjs << o;
    }

    emit objectsUnassigned(removedObjs);
}

void ObjectListWidget::assign(RamObject *obj)
{
    if (m_editMode == NoEdit)
        return;

    RamObjectModel *objList = qobject_cast<RamObjectModel*>( ui_objectView->objectModel() );
    if (objList) {
        objList->appendObject(obj->uuid());
        emit objectAssigned(obj);
    }
}

void ObjectListWidget::assignObject(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent)

    if (!m_useAssignList) return;

    RamObjectSortFilterProxyModel *model = qobject_cast<RamObjectSortFilterProxyModel*>( ui_objectView->model() );
    if (!model) return;

    for (int i = first ; i <= last; i++)
    {
        RamObject *assignedObj = model->get(i);
        ui_assignMenu->setObjectVisible(assignedObj, false);
    }
}

void ObjectListWidget::unassignObject(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent)

    if (!m_useAssignList) return;

    RamObjectSortFilterProxyModel *model = qobject_cast<RamObjectSortFilterProxyModel*>( ui_objectView->model() );

    for (int i = first ; i <= last; i++)
    {
        RamObject *assignedObj = model->get(i);
        if (!assignedObj) continue;
        ui_assignMenu->setObjectVisible(assignedObj, true);
    }
}

void ObjectListWidget::setSearchFocus()
{
    ui_objectView->releaseKeyboard();
    ui_searchEdit->setFocus();
}

void ObjectListWidget::setupUi(bool editableObjects, RamUser::UserRole editRole)
{
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    auto mainLayout = DuUI::addBoxLayout(Qt::Vertical, this);
    mainLayout->setContentsMargins(0,0,0,0);

    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    buttonsLayout->setSpacing(3);
    buttonsLayout->setContentsMargins(0,3,0,0);

    ui_title = new QLabel(this);
    ui_title->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    buttonsLayout->addWidget(ui_title);

    ui_filterBox = new RamObjectComboBox(this);
    ui_filterBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    buttonsLayout->addWidget(ui_filterBox);

    ui_removeButton = new QToolButton(this);
    ui_removeButton->setIcon(DuIcon(":/icons/remove"));
    ui_removeButton->setIconSize(QSize(12,12));
    buttonsLayout->addWidget(ui_removeButton);

    ui_addButton = new QToolButton(this);
    ui_addButton->setIcon(DuIcon(":/icons/add"));
    ui_addButton->setIconSize(QSize(12,12));
    buttonsLayout->addWidget(ui_addButton);

    mainLayout->addLayout(buttonsLayout);

    ui_searchEdit = new DuQFSearchEdit(this);
    mainLayout->addWidget(ui_searchEdit);

    ui_objectView = new RamObjectView(RamObjectView::List, this);
    ui_objectView->setEditableObjects(editableObjects, editRole);
    mainLayout->addWidget(ui_objectView);

    mainLayout->setStretch(0, 0);
    mainLayout->setStretch(1, 0);
    mainLayout->setStretch(2, 0);
    mainLayout->setStretch(3, 100);

    this->setLayout(mainLayout);

    setEditable(true);
    // Hide filters until at least one is added
    setFilterList(nullptr);

    ui_assignMenu = new RamObjectMenu(false, this);
    ui_assignMenu->addCreateButton();
}

void ObjectListWidget::connectEvents()
{
    // add & remove buttons
    connect(ui_addButton, &QToolButton::clicked, this, &ObjectListWidget::add);
    connect(ui_assignMenu, &RamObjectMenu::createTriggered, this, &ObjectListWidget::add);
    connect(ui_removeButton, &QToolButton::clicked, this, &ObjectListWidget::removeSelectedObjects);
    connect(ui_assignMenu,SIGNAL(assigned(RamObject*)),this,SLOT(assign(RamObject*)));
    // search
    connect(ui_searchEdit, &DuQFSearchEdit::changing, ui_objectView, &RamObjectView::search);
    connect(ui_searchEdit, &DuQFSearchEdit::changed, ui_objectView, &RamObjectView::search);
    // filters
    connect(ui_filterBox, &RamObjectComboBox::currentObjectChanged, this, &ObjectListWidget::setFilter);
    // Relay list signals
    connect(ui_objectView, &RamObjectView::objectSelected, this, &ObjectListWidget::objectSelected);

    // Shortcuts
    QShortcut *s = new QShortcut(QKeySequence(QKeySequence::Delete), ui_objectView, nullptr, nullptr, Qt::WidgetWithChildrenShortcut );
    connect(s, SIGNAL(activated()), this, SLOT(removeSelectedObjects()));
    s = new QShortcut(QKeySequence(QKeySequence::Find), this, nullptr, nullptr, Qt::WidgetWithChildrenShortcut );
    connect(s, SIGNAL(activated()), this, SLOT(setSearchFocus()));
}
