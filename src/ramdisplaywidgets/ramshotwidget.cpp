#include "ramshotwidget.h"

RamShotWidget::RamShotWidget(RamShot *shot, QWidget *parent) :
    RamObjectWidget(shot, parent)
{
    _shot = shot;
    setUserEditRole(RamUser::Lead);

    ShotEditWidget *sw = new ShotEditWidget(this);
    sw->setShot(shot);
    setEditWidget(sw);
    this->dockEditWidget()->setIcon(":/icons/shot");

    setIcon(":/icons/shot");

    completeUi();
    connectEvents();

    shotChanged();
}

RamShot *RamShotWidget::shot() const
{
    return _shot;
}

void RamShotWidget::exploreClicked()
{
    explore(Ramses::instance()->path(_shot));
}

void RamShotWidget::shotChanged()
{
    m_sequenceLabel->setText( _shot->sequence()->name() + " | " + _shot->shortName() );
    RamProject *project = _shot->project();
    QString duration = QString::number( _shot->duration() ) + " s / ";
    duration = duration + QString::number( round( _shot->duration() * project->framerate() ) ) + " frames @ " + QString::number(project->framerate()) + " fps";
    m_durationLabel->setText(duration);
}

void RamShotWidget::completeUi()
{
    QString detailsStyle = "color: " + DuUI::getColor("medium-grey").name() + ";";
    detailsStyle += "font-style: italic; font-weight: 300;";
    detailsStyle += "padding-right:" + QString::number(DuUI::getSize("padding","medium")) + "px;";

    m_sequenceLabel = new QLabel();
    m_sequenceLabel->setStyleSheet(detailsStyle);
    primaryContentLayout->addWidget(m_sequenceLabel);

    m_durationLabel = new QLabel();
    m_durationLabel->setStyleSheet(detailsStyle);
    primaryContentLayout->addWidget(m_durationLabel);

    setPrimaryContentHeight(50);
    showExploreButton(true);
}

void RamShotWidget::connectEvents()
{
    connect(_shot, &RamShot::changed, this, &RamShotWidget::shotChanged);
}
