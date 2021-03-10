#ifndef TEMPLATESTEPEDITWIDGET_H
#define TEMPLATESTEPEDITWIDGET_H

#include "ui_templatestepeditwidget.h"
#include "ramses.h"

class TemplateStepEditWidget : public QWidget, private Ui::TemplateStepEditWidget
{
    Q_OBJECT

public:
    explicit TemplateStepEditWidget(QWidget *parent = nullptr);

    RamStep *step() const;
    void setStep(RamStep *step);

private slots:
    void update();
    void revert();
    bool checkInput();
    void stepDestroyed(QObject *o);
    void dbiLog(DuQFLog m);

private:
    RamStep *_step;
    QMetaObject::Connection _currentStepConnection;
};

#endif // TEMPLATESTEPEDITWIDGET_H
