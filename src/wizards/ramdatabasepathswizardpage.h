#ifndef RAMDATABASEPATHSWIZARDPAGE_H
#define RAMDATABASEPATHSWIZARDPAGE_H

#include "duwidgets/dufolderselectorwidget.h"
#include <QWizardPage>

class RamDatabasePathsWizardPage : public QWizardPage
{
public:
    RamDatabasePathsWizardPage(QWidget *parent = nullptr);

    virtual bool isComplete() const override;
    virtual bool validatePage() override;
    virtual void initializePage() override;

    QString dbFilePath() const { return ui_ramsesFileSelector->path(); }
    QString projectPath() const { return ui_projectPathSelector->path(); }

private:
    void setupUi();
    void connectEvents();

    DuFolderSelectorWidget *ui_ramsesFileSelector;
    DuFolderSelectorWidget *ui_projectPathSelector;
};

#endif // RAMDATABASEPATHSWIZARDPAGE_H
