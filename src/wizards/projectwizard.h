#ifndef PROJECTWIZARD_H
#define PROJECTWIZARD_H

#include <QWizard>
#include <QDateEdit>

#include "duwidgets/dufolderselectorwidget.h"
#include "frameratewidget.h"
#include "resolutionwidget.h"

class ProjectWizard : public QWizard
{
    Q_OBJECT
public:
    ProjectWizard(bool team = false, QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());

private:
    void setupUi();

    QWizardPage *createPathsPage();
    QWizardPage *createProjectSettingsPage();

    DuFolderSelectorWidget *ui_ramsesFileSelector;
    DuFolderSelectorWidget *ui_projectPathSelector;
    ResolutionWidget *ui_resolutionWidget;
    FramerateWidget *ui_framerateWidget;
    QDateEdit *ui_deadlineEdit;

    bool _isTeamProject;
};

#endif // PROJECTWIZARD_H
