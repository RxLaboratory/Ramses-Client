#ifndef TEMPLATESTEPEDITWIDGET_H
#define TEMPLATESTEPEDITWIDGET_H

#include <QFormLayout>

#include "duwidgets/ducombobox.h"
#include "duwidgets/dudoublespinbox.h"
#include "objecteditwidget.h"
#include "duwidgets/ducolorselector.h"
#include "ramtemplatestep.h"

class TemplateStepEditWidget : public ObjectEditWidget
{
    Q_OBJECT

public:
    explicit TemplateStepEditWidget(QWidget *parent = nullptr);
    explicit TemplateStepEditWidget(RamTemplateStep *templateStep, QWidget *parent = nullptr);

    RamTemplateStep *step() const;

protected:
    virtual void reInit(RamObject *o) override;

private slots:
    void updateEstimationSuffix();
    void setType(int t);
    void setColor(QColor c);
    void setEstimationType(int t);
    void setVeryEasy(double e);
    void setEasy(double e);
    void setMedium(double e);
    void setHard(double e);
    void setVeryHard(double e);

private:
    RamTemplateStep *m_step;

    void setupUi();
    void connectEvents();

    // UI Controls
    DuComboBox *ui_typeBox;
    QLabel *ui_estimationLabel;
    QWidget *ui_estimationWidget;
    DuComboBox *ui_estimationTypeBox;
    QLabel *ui_estimationTypeLabel;
    DuDoubleSpinBox *ui_veryEasyEdit;
    DuDoubleSpinBox *ui_easyEdit;
    DuDoubleSpinBox *ui_mediumEdit;
    DuDoubleSpinBox *ui_hardEdit;
    DuDoubleSpinBox *ui_veryHardEdit;
    DuColorSelector *ui_colorSelector;
};

#endif // TEMPLATESTEPEDITWIDGET_H
