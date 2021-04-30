#include "ramstepwidget.h"

RamStepWidget::RamStepWidget(RamStep *step, QWidget *parent):
    RamObjectWidget(step, parent)
{
    m_step = step;

    StepEditWidget *sw = new StepEditWidget(step, this);
    setEditWidget(sw);

    setUserEditRole(RamUser::ProjectAdmin);

    if (step->isTemplate()) this->setTitle( step->name() + " [Template]");

    stepChanged(step);
    connect(step, &RamObject::changed, this, &RamStepWidget::stepChanged);
}

RamStep *RamStepWidget::step() const
{
    return m_step;
}

void RamStepWidget::stepChanged(RamObject *obj)
{
    RamStep *step = (RamStep*)obj;
    switch( step->type() )
    {
    case RamStep::PreProduction:
        setIcon(":/icons/project");
        break;
    case RamStep::AssetProduction:
        setIcon(":/icons/asset");
        break;
    case RamStep::ShotProduction:
        setIcon(":/icons/shot");
        break;
    case RamStep::PostProduction:
        setIcon(":/icons/film");
        break;
    default:
        setIcon(":/icons/step");
    }
}
