#ifndef RAMPROJECTSWIZARDPAGE_H
#define RAMPROJECTSWIZARDPAGE_H

#include <QWizardPage>
#include <QBoxLayout>

#include "duwidgets/dulisteditview.h"
#include "ramjsonobjectmodel.h"

class RamProjectsWizardPage : public QWizardPage
{
    Q_OBJECT
public:
    RamProjectsWizardPage(RamJsonObjectModel *projects, QWidget *parent = nullptr);

    void reinit();
    virtual void initializePage() override;

    QHash<QString, RamJsonObjectModel*> userAssignments() { return _userAssignments; };

private slots:
    void editProject(const QModelIndex &index);

private:
    void setupUi();
    void connectEvents();

    RamJsonObjectModel *_projects;
    DuListEditView *ui_projectsList;

    RamJsonObjectModel *_users = nullptr;
    RamJsonObjectModel *_assignList = nullptr;
    DuListEditView *ui_userList = nullptr;
    QBoxLayout *ui_layout;

    bool _initialized = false;

    QString _currentUuid;
    QHash<QString, RamJsonObjectModel*> _userAssignments;

    void addUserToModel(const QJsonValue &userVal, RamJsonObjectModel *model);
};

#endif // RAMPROJECTSWIZARDPAGE_H
