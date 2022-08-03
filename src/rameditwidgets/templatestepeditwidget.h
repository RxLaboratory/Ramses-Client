#ifndef TEMPLATESTEPEDITWIDGET_H
#define TEMPLATESTEPEDITWIDGET_H

#include <QComboBox>
#include <QFormLayout>

#include "duqf-widgets/autoselectdoublespinbox.h"
#include "objecteditwidget.h"
#include "ramses.h"
#include "duqf-widgets/duqfcolorselector.h"

class TemplateStepEditWidget : public ObjectEditWidget
{
    Q_OBJECT

public:
    explicit TemplateStepEditWidget(QWidget *parent = nullptr);
    explicit TemplateStepEditWidget(RamTemplateStep *templateStep, QWidget *parent = nullptr);

    RamStep *step() const;

public slots:
    void setObject(RamObject *obj) override;

protected slots:
    void update() override;

private slots:
    void updateEstimationSuffix();

private:
    RamStep *m_step;

    void setupUi();
    void connectEvents();

    // UI Controls
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
    DuQFColorSelector *ui_colorSelector;
};

#endif // TEMPLATESTEPEDITWIDGET_H
