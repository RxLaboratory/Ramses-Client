#ifndef RAMOBJECTPROPERTIESWIZARDPAGE_H
#define RAMOBJECTPROPERTIESWIZARDPAGE_H

#include <QWizardPage>

#include "ramobjectpropertieswidget.h"

class RamObjectPropertiesWizardPage : public QWizardPage
{
public:
    RamObjectPropertiesWizardPage(QWidget *parent = nullptr);

    virtual bool validatePage() override;

    QString name() const { return ui_propertiesWidget->name(); }
    QString shortName() const { return ui_propertiesWidget->shortName(); }
    QString comment() const { return ui_propertiesWidget->comment(); }
    QColor color() const { return ui_propertiesWidget->color(); }

private:
    void setupUi();
    void connectEvents();

    RamObjectPropertiesWidget *ui_propertiesWidget;
};

#endif // RAMOBJECTPROPERTIESWIZARDPAGE_H
