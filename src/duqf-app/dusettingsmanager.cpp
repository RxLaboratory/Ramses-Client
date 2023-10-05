#include "dusettingsmanager.h"

const QString DuSettingsManager::UI_FOCUS_COLOR = QStringLiteral("UI/focusColor");
const QString DuSettingsManager::UI_WINDOW_GEOMETRY = QStringLiteral("UI/windowGeometry");
const QString DuSettingsManager::UI_WINDOW_STATE = QStringLiteral("UI/windowState");
const QString DuSettingsManager::NV_CURVATURE = QStringLiteral("NV/curvature");;

DuSettingsManager *DuSettingsManager::_instance = nullptr;

DuSettingsManager *DuSettingsManager::instance()
{
    if (!DuSettingsManager::_instance) DuSettingsManager::_instance = new DuSettingsManager();
    return DuSettingsManager::_instance;
}

QColor DuSettingsManager::uiFocusColor(ColorVariant v) const
{
    QColor c = m_settings.value(UI_FOCUS_COLOR, QColor(165,38,196)).value<QColor>();
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

float DuSettingsManager::nvCurvature() const
{
    return m_settings.value(NV_CURVATURE, .5).toFloat();
}

QByteArray DuSettingsManager::uiWindowGeometry() const
{
    return m_settings.value(UI_WINDOW_GEOMETRY).toByteArray();
}

QByteArray DuSettingsManager::uiWindowState() const
{
    return m_settings.value(UI_WINDOW_STATE).toByteArray();
}

void DuSettingsManager::setUIFocusColor(const QColor &color)
{
    m_settings.setValue(UI_FOCUS_COLOR, color);
}

void DuSettingsManager::setNVCurvature(float c)
{
    // If this is a %
    if (c >= 1) c /= 100;
    
    m_settings.setValue(NV_CURVATURE, c);
    emit nvCurvatureChanged(c);
}

void DuSettingsManager::saveUIWindowState(const QByteArray &geometry, const QByteArray &state)
{
    m_settings.setValue(UI_WINDOW_GEOMETRY, geometry);
    m_settings.setValue(UI_WINDOW_STATE, state);
}

DuSettingsManager::DuSettingsManager(QObject *parent)
    : QObject{parent}
{

}
