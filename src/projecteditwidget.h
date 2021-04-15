#ifndef PROJECTEDITWIDGET_H
#define PROJECTEDITWIDGET_H

#include <QSpinBox>
#include <QDoubleSpinBox>

#include "objecteditwidget.h"
#include "ramses.h"
#include "resolutionwidget.h"
#include "frameratewidget.h"
#include "duqf-widgets/duqffolderselectorwidget.h"

class ProjectEditWidget : public ObjectEditWidget
{
    Q_OBJECT

public:
    explicit ProjectEditWidget(QWidget *parent = nullptr);
    explicit ProjectEditWidget(RamProject *project, QWidget *parent = nullptr);

    RamProject *project() const;
    void setProject(RamProject *project);

protected slots:
    void update() Q_DECL_OVERRIDE;

private slots:
    void updateFolderLabel(QString path);
    void projectChanged(RamObject *o);

private:
    RamProject *_project;

    void setupUi();
    void connectEvents();

    DuQFFolderSelectorWidget *folderSelector;
    QLabel *folderLabel;
    ResolutionWidget *resolutionWidget;
    FramerateWidget *framerateWidget;
};

#endif // PROJECTEDITWIDGET_H
