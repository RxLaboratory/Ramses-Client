#ifndef RAMPROJECTSWIZARDPAGE_H
#define RAMPROJECTSWIZARDPAGE_H

#include <QWizardPage>

#include "duwidgets/dulisteditview.h"
#include "ramjsonobjectmodel.h"

class RamProjectsWizardPage : public QWizardPage
{
    Q_OBJECT
public:
    RamProjectsWizardPage(RamJsonObjectModel *projects, QWidget *parent = nullptr);

    void reinit();
    virtual void initializePage() override;

private slots:
    void editProject(const QModelIndex &index);

private:
    void setupUi();
    void connectEvents();

    RamJsonObjectModel *_projects;
    DuListEditView *ui_projectsList;
    bool _initialized = false;
};

#endif // RAMPROJECTSWIZARDPAGE_H
