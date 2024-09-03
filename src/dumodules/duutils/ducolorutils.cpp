#include "ducolorutils.h"

QColor DuColorUtils::darker(const QColor &color, int factor)
{
    QColor newColor = color.darker(factor);

    if (color.saturation() < 10)
        return newColor;

    int h = newColor.hue();
    if (h < 240) h += 15;
    else h -= 15;

    int s = newColor.saturation();
    s = std::min(255, s+15);

    newColor.setHsv(h, s, newColor.value());
    return newColor;
}

QColor DuColorUtils::lighter(const QColor &color, int factor)
{
    QColor newColor = color.lighter(factor);

    if (color.saturation() < 10)
        return newColor;

    int h = newColor.hue();
    if (h < 240) h -= 15;
    else h += 15;
    h = std::min(359, h);
    h = std::max(0, h);

    int s = newColor.saturation();
    s = std::max(0, s-15);

    newColor.setHsv(h, s, newColor.value());
    return newColor;
}

QColor DuColorUtils::shiftHue(const QColor &color, int offset)
{
    QColor newColor = color;
    int h = newColor.hue() + offset;
    if (h > 359) h -= 360;
    else if (h < 0) h += 360;

    newColor.setHsv( h, color.saturation(), color.value());
    return newColor;
}

QColor DuColorUtils::colorFromString(const QString &str, int saturation, int value)
{
    int hue= 0;
    for(const auto &c: str)
        hue += c.unicode();
    hue %= 360;

    return QColor::fromHsv(hue, saturation, value);
}

QColor DuColorUtils::colorFromDate(const QDateTime &date, int saturation, int value)
{
    QDateTime marx = QDate(1818,05,05).startOfDay();
    qint64 hue = marx.secsTo(date);
    hue /= 1800;
    return QColor::fromHsv(hue % 360, saturation, value);
}

QColor DuColorUtils::colorFromHue(int hue, int saturation, int value)
{
    return QColor::fromHsv(hue, saturation, value);
}
