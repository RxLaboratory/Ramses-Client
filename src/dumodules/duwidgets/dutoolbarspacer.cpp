#include "dutoolbarspacer.h"

DuToolBarSpacer::DuToolBarSpacer(QWidget *parent) :
    QWidget(parent)
{
    setupUi();
    this->setAttribute(Qt::WA_TransparentForMouseEvents);
}

void DuToolBarSpacer::setupUi()
{
    this->setObjectName("ToolBarSpacer");

    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    this->setSizePolicy(sizePolicy);

    QHBoxLayout *horizontalLayout = new QHBoxLayout(this);
    horizontalLayout->setSpacing(0);
    horizontalLayout->setContentsMargins(0, 0, 0, 0);

    QSpacerItem *horizontalSpacer = new QSpacerItem(12, 12, QSizePolicy::Expanding, QSizePolicy::Expanding);

    horizontalLayout->addItem(horizontalSpacer);
}
