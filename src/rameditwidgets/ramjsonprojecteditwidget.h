#ifndef RAMJSONPROJECTEDITWIDGET_H
#define RAMJSONPROJECTEDITWIDGET_H

#include <QDateEdit>

#include "frameratewidget.h"
#include "objectlistwidget.h"
#include "ramjsonobjecteditwidget.h"
#include "resolutionwidget.h"

class RamProject;

class RamJsonProjectEditWidget : public RamJsonObjectEditWidget
{
    Q_OBJECT
public:
    RamJsonProjectEditWidget(const QString &uuid = "", QWidget *parent = nullptr);
    virtual QJsonObject data() const override;
    virtual void setData(const QJsonObject &obj) override;

private slots:
    void setProject();

private:
    void setupUi();
    void setupMainTab();
    void setupUsersTab();
    void connectEvents();

    ResolutionWidget *ui_resolutionWidget;
    FramerateWidget *ui_framerateWidget;
    QDoubleSpinBox *ui_parBox;
    QDateEdit *ui_deadlineEdit;
    ObjectListWidget *ui_userList = nullptr;

    RamProject *_project = nullptr;
};

#endif // RAMJSONPROJECTEDITWIDGET_H
