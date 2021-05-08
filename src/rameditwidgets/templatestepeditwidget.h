#ifndef TEMPLATESTEPEDITWIDGET_H
#define TEMPLATESTEPEDITWIDGET_H

#include <QComboBox>

#include "objecteditwidget.h"
#include "ramses.h"

class TemplateStepEditWidget : public ObjectEditWidget
{
    Q_OBJECT

public:
    explicit TemplateStepEditWidget(QWidget *parent = nullptr);
    explicit TemplateStepEditWidget(RamStep *templateStep, QWidget *parent = nullptr);

    RamStep *step() const;

public slots:
    void setObject(RamObject *obj) Q_DECL_OVERRIDE;

protected slots:
    void update() Q_DECL_OVERRIDE;

private slots:

private:
    RamStep *_step;

    void setupUi();
    void connectEvents();

    QComboBox *ui_typeBox;
};

#endif // TEMPLATESTEPEDITWIDGET_H
