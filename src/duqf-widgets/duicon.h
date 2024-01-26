#ifndef DUICON_H
#define DUICON_H

#include "duqf-widgets/dusvgiconengine.h"
#include <QIcon>

class DuIcon : public QIcon
{
public:
    DuIcon();
    DuIcon(DuSVGIconEngine *engine);
    DuIcon(const QString &file);

    DuSVGIconEngine *engine() const;

private:
    DuSVGIconEngine *_engine;
};

#endif // DUICON_H
