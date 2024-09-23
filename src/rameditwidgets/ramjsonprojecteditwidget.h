#ifndef RAMJSONPROJECTEDITWIDGET_H
#define RAMJSONPROJECTEDITWIDGET_H

#include <QDateEdit>

#include "frameratewidget.h"
#include "objectlistwidget.h"
#include "ramjsonobjecteditwidget.h"
#include "resolutionwidget.h"
#include "duwidgets/dudoublespinbox.h"
#include "duwidgets/dudateedit.h"

class RamProject;

class RamJsonProjectEditWidget : public RamJsonObjectEditWidget
{
    Q_OBJECT
public:
    RamJsonProjectEditWidget(const QString &uuid = "", QWidget *parent = nullptr);
    virtual QJsonObject data() const override;
    virtual void setData(const QJsonObject &obj) override;

    void hideUsersTab();

private slots:
    void setProject();

private:
    void setupUi();
    void setupMainTab();
    void setupUsersTab();
    void connectEvents();

    // Cache
    qreal _par;
    qreal _fps;
    QDate _deadline;

    ResolutionWidget *ui_resolutionWidget;
    FramerateWidget *ui_framerateWidget;
    DuDoubleSpinBox *ui_parBox;
    DuDateEdit *ui_deadlineEdit;
    ObjectListWidget *ui_userList = nullptr;

    RamProject *_project = nullptr;
};

#endif // RAMJSONPROJECTEDITWIDGET_H
