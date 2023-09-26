#include "dusettingsmanager.h"

const QString DuSettingsManager::FOCUS_COLOR = QStringLiteral("focusColor");
const QString DuSettingsManager::NODESVIEW_CURVATURE = QStringLiteral("nodesView/curvature");;

DuSettingsManager *DuSettingsManager::_instance = nullptr;

DuSettingsManager *DuSettingsManager::instance()
{
    if (!DuSettingsManager::_instance) DuSettingsManager::_instance = new DuSettingsManager();
    return DuSettingsManager::_instance;
}

QColor DuSettingsManager::focusColor(ColorVariant v) const
{
    QColor c = m_settings.value(FOCUS_COLOR, QColor(165,38,196)).value<QColor>();
    if (v == DarkerColor) {
        int lightness = std::min( 55, c.lightness() );
        int saturation = std::min( 100, c.hslSaturation() );
        int hue = c.hslHue();
        c.setHsl(hue, saturation, lightness);
    }
    else if (v == LighterColor) {
        int lightness = std::max( 200, c.lightness() );
        int saturation = std::min( 100, c.hslSaturation() );
        int hue = c.hslHue();
        c.setHsl(hue, saturation, lightness);
    }
    return c;
}

float DuSettingsManager::nodesViewCurvature() const
{
    return m_settings.value(NODESVIEW_CURVATURE, .5).toFloat();
}

void DuSettingsManager::setFocusColor(const QColor &color)
{
    m_settings.setValue(FOCUS_COLOR, color);
}

void DuSettingsManager::setNodesViewCurvature(float c)
{
    // If this is a %
    if (c >= 1) c /= 100;

    m_settings.setValue(NODESVIEW_CURVATURE, c);
    emit nodesViewCurvatureChanged(c);
}

DuSettingsManager::DuSettingsManager(QObject *parent)
    : QObject{parent}
{

}
