#ifndef DUCOLORUTILS_H
#define DUCOLORUTILS_H

#include "qdatetime.h"
#include <QColor>

namespace DuColorUtils
{
/**
 * @brief Creates a darker color like QColor::Darker()
 * but adjusting the hue and saturation a bit
 * to make it look nicer.
 * @param color The color to adjust.
 * @param factor The factor in %
 * @return The new color.
 */
QColor darker(const QColor &color, int factor = 140);

/**
 * @brief Creates a lighter color like QColor::lighter()
 * but adjusting the hue and saturation a bit
 * to make it look nicer.
 * @param color The color to adjust.
 * @param factor The factor in %
 * @return The new color.
 */
QColor lighter(const QColor &color, int factor = 110);

/**
 * @brief shiftHue Offsets the hue
 * @param color The color to adjust.
 * @param offset The offset
 * @return The new color.
 */
QColor shiftHue(const QColor &color, int offset = 30);

QColor colorFromString(const QString &str, int saturation = 180, int value = 100);
QColor colorFromDate(const QDateTime &date, int saturation = 180, int value = 100);
QColor colorFromHue(int hue, int saturation = 180, int value = 100);
};

#endif // DUCOLORUTILS_H
