#ifndef STEPEDITWIDGET_H
#define STEPEDITWIDGET_H

#include <QComboBox>
#include <QSplitter>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QToolButton>
#include <QListWidget>
#include <QMenu>

#include "objecteditwidget.h"
#include "ramses.h"
#include "objectlisteditwidget.h"
#include "duqf-widgets/duqffolderdisplaywidget.h"

class StepEditWidget : public ObjectEditWidget
{
    Q_OBJECT
public:
    StepEditWidget(QWidget *parent = nullptr);
    StepEditWidget(RamStep *s, QWidget *parent = nullptr);

    RamStep *step() const;

public slots:
    void setObject(RamObject *obj) Q_DECL_OVERRIDE;

protected slots:
    void update() Q_DECL_OVERRIDE;

private slots:
    void createUser();
    void createApplication();

private:
    void setupUi();
    void connectEvents();

    RamStep *m_step;

    QComboBox *m_typeBox;
    DuQFFolderDisplayWidget *m_folderWidget;
    ObjectListEditWidget *m_userList;
    ObjectListEditWidget *m_applicationList;

};

#endif // STEPEDITWIDGET_H
