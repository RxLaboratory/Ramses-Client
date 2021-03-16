#ifndef APPLICATIONSMANAGERWIDGET_H
#define APPLICATIONSMANAGERWIDGET_H

#include "listmanagerwidget.h"
#include "applicationeditwidget.h"

class ApplicationsManagerWidget : public ListManagerWidget
{
    Q_OBJECT
public:
    ApplicationsManagerWidget(QWidget *parent = nullptr);

protected slots:
    void currentDataChanged(QVariant data) Q_DECL_OVERRIDE;
    void createItem() Q_DECL_OVERRIDE;
    void removeItem(QVariant data) Q_DECL_OVERRIDE;

private slots:
    void newApplication(RamApplication *a);
    void removeApplication(RamObject *o);
    void applicationChanged();

private:
    ApplicationEditWidget *applicationWidget;
};

#endif // APPLICATIONSMANAGERWIDGET_H
