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
#include "ramjsonobjecteditwidget.h"
#include "ramobjectcombobox.h"

class RamJsonStepEditWidget: public RamJsonObjectEditWidget
{
    Q_OBJECT
public:
    RamJsonStepEditWidget(const QString &uuid = "", QWidget *parent = nullptr);
    virtual QJsonObject data() const override;
    virtual void setData(const QJsonObject &obj, const QString &uuid) override;

protected:
    virtual void updateUuid() override;

private slots:
    void updateEstimationMethod();

private:
    void setupUi();
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

};

#endif // RAMJSONSTEPEDITWIDGET_H
