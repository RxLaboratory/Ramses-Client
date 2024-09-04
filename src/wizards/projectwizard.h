#ifndef PROJECTWIZARD_H
#define PROJECTWIZARD_H

#include <QWizard>
#include <QDateEdit>

#include "duwidgets/dufolderselectorwidget.h"
#include "duwidgets/dulistmodeledit.h"
#include "frameratewidget.h"
#include "ramjsonobjectmodel.h"
#include "ramjsonstepeditwidget.h"
#include "resolutionwidget.h"

class ProjectWizard : public QWizard
{
    Q_OBJECT
public:
    ProjectWizard(bool team = false, QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());

private slots:
    void editStep(const QModelIndex &index);

private:
    void setupUi();
    void connectEvents();

    QWizardPage *createPathsPage();
    QWizardPage *createProjectSettingsPage();
    QWizardPage *createPipelinePage();

    DuFolderSelectorWidget *ui_ramsesFileSelector;
    DuFolderSelectorWidget *ui_projectPathSelector;
    ResolutionWidget *ui_resolutionWidget;
    FramerateWidget *ui_framerateWidget;
    QDateEdit *ui_deadlineEdit;
    DuListModelEdit *ui_stepList;

    RamJsonObjectModel *_steps;

    bool _isTeamProject;
};

#endif // PROJECTWIZARD_H
