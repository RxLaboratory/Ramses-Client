#ifndef RAMJSONSTEPEDITWIDGET_H
#define RAMJSONSTEPEDITWIDGET_H

#include <QSplitter>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QToolButton>
#include <QFormLayout>
#include <QLabel>
#include <QCheckBox>

#include "duwidgets/autoselectdoublespinbox.h"
#include "duwidgets/ducombobox.h"
#include "progresswidget.h"
#include "ramjsonobjecteditwidget.h"
#include "ramobjectcombobox.h"

class RamJsonStepEditWidget: public RamJsonObjectEditWidget
{
    Q_OBJECT
public:
    RamJsonStepEditWidget(const QString &uuid = "", QWidget *parent = nullptr);
    virtual QJsonObject data() const override;
    virtual void setData(const QJsonObject &obj) override;

private slots:
    void updateEstimationMethod();
    void changeUser(RamObject *userObj);

private:
    void setupUi();
    void setupMainTab();
    void setupEstimTab();
    void setupPublishTab();
    void setProject();
    void connectEvents();

    DuComboBox *ui_typeBox;
    DuComboBox *ui_estimationTypeBox;
    QLabel *ui_estimationTypeLabel;
    DuRichTextEdit *ui_publishSettingsEdit;
    AutoSelectDoubleSpinBox *ui_veryEasyEdit;
    AutoSelectDoubleSpinBox *ui_easyEdit;
    AutoSelectDoubleSpinBox *ui_mediumEdit;
    AutoSelectDoubleSpinBox *ui_hardEdit;
    AutoSelectDoubleSpinBox *ui_veryHardEdit;
    QCheckBox *ui_estimationMultiplierCheckBox;
    RamObjectComboBox *ui_estimationMultiplierBox;

    QWidget *ui_estimWidget;
    QLabel *ui_estimLabel;
    RamObjectComboBox *ui_userBox;
    QWidget *ui_completionWidget;
    ProgressWidget *ui_progressWidget;
    QLabel *ui_completionLabel;
    QWidget *ui_statesWidget;
    QWidget *ui_difficultiesWidget;
    QFormLayout *ui_statesLayout;
    QFormLayout *ui_difficultiesLayout;

};

#endif // RAMJSONSTEPEDITWIDGET_H
