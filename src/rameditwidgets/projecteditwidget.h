#ifndef PROJECTEDITWIDGET_H
#define PROJECTEDITWIDGET_H

#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QDateTimeEdit>

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

public slots:
    void setObject(RamObject *obj) override;

protected slots:
    void update() override;

private slots:
    void updateFolderLabel(QString path);
    void currentUserChanged(RamUser *user);

private:
    RamProject *_project;

    void setupUi();
    void connectEvents();

    DuQFFolderSelectorWidget *ui_folderSelector;
    QLabel *ui_folderLabel;
    ResolutionWidget *ui_resolutionWidget;
    FramerateWidget *ui_framerateWidget;
    QDateTimeEdit *ui_deadlineEdit;
};

#endif // PROJECTEDITWIDGET_H
