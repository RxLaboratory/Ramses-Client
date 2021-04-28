#include "templatestepeditwidget.h"

TemplateStepEditWidget::TemplateStepEditWidget(QWidget *parent) :
    ObjectEditWidget(parent)
{
    setupUi();
    connectEvents();

    setObject(nullptr);
}

TemplateStepEditWidget::TemplateStepEditWidget(RamStep *templateStep, QWidget *parent) :
    ObjectEditWidget(templateStep, parent)
{
    setupUi();
    connectEvents();

    setObject(templateStep);
}

RamStep *TemplateStepEditWidget::step() const
{
    return _step;
}

void TemplateStepEditWidget::setObject(RamObject *obj)
{
    RamStep *step = (RamStep*)obj;

    this->setEnabled(false);

    ObjectEditWidget::setObject(step);
    _step = step;

    QSignalBlocker b1(ui_typeBox);

    ui_typeBox->setCurrentIndex(1);

    if (!step) return;

    if (step->type() == RamStep::PreProduction) ui_typeBox->setCurrentIndex(0);
    else if (step->type() == RamStep::AssetProduction) ui_typeBox->setCurrentIndex(1);
    else if (step->type() == RamStep::ShotProduction) ui_typeBox->setCurrentIndex(2);
    else if (step->type() == RamStep::PostProduction) ui_typeBox->setCurrentIndex(3);

    this->setEnabled(Ramses::instance()->isAdmin());

    _objectConnections << connect(step, &RamStep::changed, this, &TemplateStepEditWidget::stepChanged);
}

void TemplateStepEditWidget::update()
{
    if (!_step) return;

    updating = true;

    _step->setType(ui_typeBox->currentData().toString());

    ObjectEditWidget::update();

    updating = false;
}

void TemplateStepEditWidget::stepChanged(RamObject *o)
{
    if (updating) return;
    Q_UNUSED(o);
    setObject(_step);
}

void TemplateStepEditWidget::setupUi()
{
    QLabel *typeLabel = new QLabel("Type", this);
    mainFormLayout->addWidget(typeLabel, 2, 0);

    ui_typeBox  = new QComboBox(this);
    ui_typeBox->addItem(QIcon(":/icons/project"), "       Pre-Production", "pre");
    ui_typeBox->addItem(QIcon(":/icons/asset"), "       Asset Production", "asset");
    ui_typeBox->addItem(QIcon(":/icons/shot"), "       Shot Production", "shot");
    ui_typeBox->addItem(QIcon(":/icons/film"), "       Post-Production", "post");
    mainFormLayout->addWidget(ui_typeBox, 2, 1);

    mainLayout->addStretch();
}

void TemplateStepEditWidget::connectEvents()
{
    connect(ui_typeBox, SIGNAL(currentIndexChanged(int)), this, SLOT(update()));
}
