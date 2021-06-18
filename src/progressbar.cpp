#include "progressbar.h"

ProgressBar::ProgressBar(QWidget *parent):
    QProgressBar(parent)
{
    ProcessManager *pm = ProcessManager::instance();
    connect(pm, &ProcessManager::text, this, &QProgressBar::setFormat);
    connect(pm, &ProcessManager::progress, this, &QProgressBar::setValue);
    connect(pm, &ProcessManager::maximum, this, &QProgressBar::setMaximum);
    connect(pm, &ProcessManager::started, this, &QProgressBar::show);
    connect(pm, &ProcessManager::finished, this, &QProgressBar::hide);
    //connect(pm, SIGNAL(text(QString)), this, SLOT(repaint()));
    //connect(pm, SIGNAL(progress(int)), this, SLOT(repaint()));
    this->hide();
}
