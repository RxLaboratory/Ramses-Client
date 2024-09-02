#ifndef COLORUTILS_H
#define COLORUTILS_H

#include "qcolor.h"
namespace ColorUtils {

QColor interpolateHSL(const QColor &c1, const QColor &c2, qreal v) {
    if (v>=1) return c2;
    if (v<=0) return c1;
    return QColor::fromHsl(
        c1.hue()*(1-v) + c2.hue()*v,
        c1.saturation()*(1-v) + c2.saturation()*v,
        c1.lightness()*(1-v) + c2.lightness()*v
        );
}

QColor interpolateRGB(const QColor &c1, const QColor &c2, qreal v) {
    if (v>=1) return c2;
    if (v<=0) return c1;
    return QColor(
        c1.red()*(1-v) + c2.red()*v,
        c1.green()*(1-v) + c2.green()*v,
        c1.blue()*(1-v) + c2.blue()*v
        );
}

}

#endif // COLORUTILS_H
