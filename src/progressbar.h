#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H

#include <QProgressBar>

#include "processmanager.h"

class ProgressBar : public QProgressBar
{
public:
    ProgressBar(QWidget *parent = nullptr);
};

#endif // PROGRESSBAR_H
