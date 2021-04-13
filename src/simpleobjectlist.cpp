#include "simpleobjectlist.h"

SimpleObjectList::SimpleObjectList(QWidget *parent) : QScrollArea(parent)
{
    setupUi();
}

void SimpleObjectList::addWidget(QWidget *w)
{
    mainLayout->insertWidget( mainLayout->count() -1, w);
}

void SimpleObjectList::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        QWidget *w = childAt(event->pos());
        if (w) emit currentIndexChanged( mainLayout->indexOf(w) );
        else emit currentIndexChanged( -1 );
    }
}

void SimpleObjectList::setupUi()
{
    this->setFrameStyle(QFrame::NoFrame);
    this->setWidgetResizable(true);

    QWidget *mainWidget = new QWidget(this);

    mainLayout = new QVBoxLayout(mainWidget);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(15,15,15,15);
    mainLayout->addStretch();

    mainWidget->setLayout(mainLayout);

    this->setWidget(mainWidget);
}
