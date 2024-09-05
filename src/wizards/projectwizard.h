#ifndef PROJECTWIZARD_H
#define PROJECTWIZARD_H

#include <QWizard>
#include <QDateEdit>

#include "duwidgets/dulistmodeledit.h"
#include "frameratewidget.h"
#include "ramjsonobjectmodel.h"
#include "resolutionwidget.h"
#include "wizards/ramobjectpropertieswizardpage.h"
#include "wizards/ramprojectpathspage.h"

class ProjectWizard : public QWizard
{
    Q_OBJECT
public:
    ProjectWizard(bool team = false, QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());

protected:
    virtual void done(int r) override;

private slots:
    void editStep(const QModelIndex &index);

private:
    void setupUi();
    void connectEvents();

    QWizardPage *createProjectSettingsPage();
    QWizardPage *createPipelinePage();

    RamObjectPropertiesWizardPage *ui_detailsPage;
    RamProjectPathsPage *ui_pathsPage;

    ResolutionWidget *ui_resolutionWidget;
    FramerateWidget *ui_framerateWidget;
    QDateEdit *ui_deadlineEdit;
    DuListModelEdit *ui_stepList;

    RamJsonObjectModel *_steps;

    bool _isTeamProject;
};

#endif // PROJECTWIZARD_H
