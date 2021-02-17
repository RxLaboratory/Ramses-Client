#ifndef STEPEDITWIDGET_H
#define STEPEDITWIDGET_H

#include "ui_stepeditwidget.h"
#include "ramses.h"

class StepEditWidget : public QWidget, private Ui::StepEditWidget
{
    Q_OBJECT

public:
    explicit StepEditWidget(QWidget *parent = nullptr);

    RamStep *step() const;
    void setStep(RamStep *step);

private slots:
    void update();
    void revert();
    bool checkInput();
    void stepDestroyed(QObject *o);
    void dbiLog(QString m, LogUtils::LogType t);

private:
    RamStep *_step;
    QMetaObject::Connection _currentStepConnection;
};

#endif // STEPEDITWIDGET_H
