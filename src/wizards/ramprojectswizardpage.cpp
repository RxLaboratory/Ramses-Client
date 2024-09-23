#include "ramprojectswizardpage.h"

#include "duapp/duui.h"
#include "ramjsonprojecteditwidget.h"
#include "mainwindow.h"

RamProjectsWizardPage::RamProjectsWizardPage(RamJsonObjectModel *projects, QWidget *parent):
    QWizardPage(parent),
    _projects(projects)
{
    setupUi();
    connectEvents();
}

void RamProjectsWizardPage::reinit()
{
    if (_projects->rowCount() > 0)
        _projects->removeRows(0, _projects->rowCount());
    _initialized = false;
    initializePage();
}

void RamProjectsWizardPage::initializePage()
{
    if (_initialized)
        return;
    _initialized = true;

    // List existing projects from the server
    QJsonObject obj = RamServerClient::i()->getProjects();
    if (!obj.value("success").toBool(false)) {
        QMessageBox::warning(this,
                             tr("Project list"),
                             tr("Can't get the project list from the server, sorry!") + "\n" + obj.value("message").toString()
                             );
        return;
    }

    const QJsonArray projectsArr = obj.value("content").toArray();

    for (const auto &projectVal: projectsArr) {
        QJsonObject serverObj = projectVal.toObject();
        QString dataStr = serverObj.value("data").toString();
        QJsonDocument dataDoc = QJsonDocument::fromJson(dataStr.toUtf8());
        QJsonObject projectObj = dataDoc.object();
        projectObj.insert(RamObject::KEY_Uuid, serverObj.value("uuid").toString());
        int i = _projects->rowCount();
        _projects->insertRows(i, 1);
        _projects->setData(_projects->index(i), projectObj);
    }
}

void RamProjectsWizardPage::editProject(const QModelIndex &index)
{
    QJsonObject data = index.data(Qt::EditRole).toJsonObject();
    QString uuid = data.value("uuid").toString();

    auto editor = new RamJsonProjectEditWidget( uuid, this);

    editor->setData(data);

    connect(editor, &RamJsonProjectEditWidget::dataEdited,
            this, [this, index] (const QJsonObject &obj) {
                _projects->setData(index, obj);
            });
    connect(this, &RamProjectsWizardPage::destroyed,
            editor, &RamJsonProjectEditWidget::deleteLater);

    // Show
    DuUI::appMainWindow()->setPropertiesDockWidget(
        editor,
        tr("Project"),
        ":/icons/project",
        true);
}

void RamProjectsWizardPage::setupUi()
{
    this->setTitle(tr("Projects"));
    this->setSubTitle(tr("Manage server projects."));

    auto layout = DuUI::createBoxLayout(Qt::Vertical);
    DuUI::centerLayout(layout, this, 200);

    ui_projectsList = new DuListEditView(tr("Projects"), tr("project"), _projects, this);
    layout->addWidget(ui_projectsList);
}

void RamProjectsWizardPage::connectEvents()
{
    connect(ui_projectsList, &DuListEditView::editing,
            this, &RamProjectsWizardPage::editProject);
}
