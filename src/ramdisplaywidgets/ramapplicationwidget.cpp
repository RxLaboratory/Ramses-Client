#include "ramapplicationwidget.h"

RamApplicationWidget::RamApplicationWidget(RamApplication *application, QWidget *parent):
    RamObjectWidget(application, parent)
{
    m_application = application;

    ApplicationEditWidget *aw = new ApplicationEditWidget(application, this);
    setEditWidget(aw);

    setUserEditRole(RamUser::ProjectAdmin);

    setIcon(":/icons/application");
}

RamApplication *RamApplicationWidget::application() const
{
    return m_application;
}
