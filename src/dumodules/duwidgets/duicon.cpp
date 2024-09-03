#include "duicon.h"
#include "duwidgets/dusvgiconengine.h"

DuIcon::DuIcon():
    QIcon( new DuSVGIconEngine() )
{
    _engine = nullptr;
}

DuIcon::DuIcon(DuSVGIconEngine *engine):
    QIcon( engine )
{
    _engine = engine;
}

DuIcon::DuIcon(const QString &file):
    QIcon( new DuSVGIconEngine(file) )
{
    _engine = nullptr;
}

DuSVGIconEngine *DuIcon::engine() const
{
    return _engine;
}
