#ifndef RAMJSONSTEPEDITWIDGET_H
#define RAMJSONSTEPEDITWIDGET_H

#include <QSplitter>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QToolButton>
#include <QFormLayout>
#include <QLabel>

#include "duwidgets/autoselectdoublespinbox.h"
#include "duwidgets/ducombobox.h"
#include "ramjsonobjecteditwidget.h"

class RamJsonStepEditWidget: public RamJsonObjectEditWidget
{
    Q_OBJECT
public:
    RamJsonStepEditWidget(QWidget *parent = nullptr);
    virtual QJsonObject data() const;
    virtual void setData(const QJsonObject &obj);

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

};

#endif // RAMJSONSTEPEDITWIDGET_H
