#include "projectlistwizardpage.h"

#include <QMessageBox>
#include <QJsonArray>
#include <QJsonDocument>

#include "duapp/duui.h"
#include "ramserverclient.h"
#include "ramproject.h"

ProjectListWizardPage::ProjectListWizardPage(QWidget *parent):
    QWizardPage(parent)
{
    setupUi();
    connectEvents();
}

void ProjectListWizardPage::initializePage()
{
    QJsonObject rep = RamServerClient::i()->getProjects();
    if (!rep.value("success").toBool(false)){
        QMessageBox::warning(this,
                             tr("Can't list projects"),
                             rep.value("message").toString("Unknown error")
                             );
        return;
    }

    const QJsonArray projects = rep.value("content").toArray();
    if (projects.isEmpty()) {
        QMessageBox::warning(this,
                             tr("Project not found"),
                             tr("It seems you're not assigned to any project on this server.\nPlease contact your Ramses administrator.")
                             );
        return;
    }

    for(const auto &p: projects) {
        QJsonObject pObj = p.toObject();

        QString uuid = pObj.value("uuid").toString();
        if (uuid == "")
            continue;

        QString dataStr = pObj.value("data").toString();
        auto dataDoc = QJsonDocument::fromJson(dataStr.toUtf8());
        QJsonObject data = dataDoc.object();

        QString sn = data.value(RamProject::KEY_ShortName).toString();
        QString n = data.value(RamProject::KEY_Name).toString();

        QStringList pName( { sn, n }) ;
        _projects.insert(uuid, pName);

        if (sn != "" && n != "") sn += " | ";
        sn += n;
        ui_projectsBox->addItem(sn, uuid);
    }
}

void ProjectListWizardPage::setupUi()
{
    this->setTitle(tr("Project selection"));
    this->setSubTitle(tr("Select a project you'd like to join on this server."));

    auto layout = DuUI::createFormLayout();
    DuUI::centerLayout(layout, this, 200);

    ui_projectsBox = new DuComboBox(this);
    layout->addRow(tr("Select project"), ui_projectsBox);

    registerField("shortName*", this, "shortName", "shortNameChanged");
    registerField("name*", this, "name", "nameChanged");
}

void ProjectListWizardPage::connectEvents()
{
    connect(ui_projectsBox, &DuComboBox::dataChanged, this, [this] (const QVariant &data) {
        QString uuid = data.toString();
        QStringList pName = _projects.value(uuid, QStringList( {"", ""}));
        _shortName = pName.first();
        _name = pName.last();
        emit nameChanged(_name);
        emit shortNameChanged(_shortName);
    });
}

void ProjectListWizardPage::setUuid(const QString &newUuid)
{
    ui_projectsBox->setCurrentData(newUuid);
    emit uuidChanged( ui_projectsBox->currentData().toString() );
}
