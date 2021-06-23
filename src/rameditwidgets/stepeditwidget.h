#ifndef STEPEDITWIDGET_H
#define STEPEDITWIDGET_H

#include <QComboBox>
#include <QSplitter>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QToolButton>
#include <QListWidget>
#include <QFormLayout>
#include <QMenu>

#include "duqf-widgets/autoselectdoublespinbox.h"
#include "objecteditwidget.h"
#include "ramses.h"
#include "objectlisteditwidget.h"
#include "duqf-widgets/duqffolderdisplaywidget.h"
#include "data-views/ramobjectlistcombobox.h"

class StepEditWidget : public ObjectEditWidget
{
    Q_OBJECT
public:
    StepEditWidget(QWidget *parent = nullptr);
    StepEditWidget(RamStep *s, QWidget *parent = nullptr);

    RamStep *step() const;

public slots:
    void setObject(RamObject *obj) override;

protected slots:
    void update() override;

private slots:
    void createUser();
    void createApplication();
    void updateEstimationSuffix();

private:
    void setupUi();
    void connectEvents();

    RamStep *m_step;

    QComboBox *ui_typeBox;
    QLabel *ui_estimationLabel;
    QWidget *ui_estimationWidget;
    QComboBox *ui_estimationTypeBox;
    QLabel *ui_estimationTypeLabel;
    AutoSelectDoubleSpinBox *ui_veryEasyEdit;
    AutoSelectDoubleSpinBox *ui_easyEdit;
    AutoSelectDoubleSpinBox *ui_mediumEdit;
    AutoSelectDoubleSpinBox *ui_hardEdit;
    AutoSelectDoubleSpinBox *ui_veryHardEdit;
    DuQFFolderDisplayWidget *m_folderWidget;
    QCheckBox *ui_estimationMultiplierCheckBox;
    RamObjectListComboBox *ui_estimationMultiplierBox;


    ObjectListEditWidget *m_userList;
    ObjectListEditWidget *m_applicationList;

};

#endif // STEPEDITWIDGET_H
