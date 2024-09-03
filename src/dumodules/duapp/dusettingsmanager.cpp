#include "dusettingsmanager.h"
#include "qcolor.h"
#include "qsize.h"

const QString DuSettingsManager::UI_FOCUS_COLOR = QStringLiteral("UI/focusColor");
const QString DuSettingsManager::UI_WINDOW_GEOMETRY = QStringLiteral("UI/windowGeometry");
const QString DuSettingsManager::UI_WINDOW_STATE = QStringLiteral("UI/windowState");
const QString DuSettingsManager::NV_CURVATURE = QStringLiteral("NV/curvature");
const QString DuSettingsManager::UI_TOOLBUTTON_STYLE = QStringLiteral("UI/toolButtonStyle");
const QString DuSettingsManager::UI_TRAYICON_MODE = QStringLiteral("UI/trayIconMode");
const QString DuSettingsManager::UI_ICON_COLOR = QStringLiteral("UI/iconColor");
const QString DuSettingsManager::UI_SHOW_TRAYICON = QStringLiteral("UI/showTrayIcon");
const QString DuSettingsManager::UI_DOCK_SIZE = QStringLiteral("UI/dockSize/");
const QString DuSettingsManager::UI_DATE_FORMAT = QStringLiteral("UI/dateFomat");
const QString DuSettingsManager::CHECK_UPDATES = QStringLiteral("checkUpdates");
const QString DuSettingsManager::DAEMON_PORT = QStringLiteral("daemonPort");

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
    emit uiFocusColorChanged(color);
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

Qt::ToolButtonStyle DuSettingsManager::uiToolButtonStyle() const
{
    int style = m_settings.value(UI_TOOLBUTTON_STYLE, Qt::ToolButtonTextBesideIcon).toInt();
    return static_cast<Qt::ToolButtonStyle>( style );
}

void DuSettingsManager::setUIToolButtonStyle(Qt::ToolButtonStyle style)
{
    m_settings.setValue(UI_TOOLBUTTON_STYLE, style);
    emit uiToolButtonStyleChanged(style);
}

void DuSettingsManager::setUIToolButtonStyle(int style)
{
    auto s = static_cast<Qt::ToolButtonStyle>( style );
    setUIToolButtonStyle(s);
}

QSize DuSettingsManager::uiDockSize(const QString &dockName) const
{
    return m_settings.value(UI_DOCK_SIZE+dockName).toSize();
}

void DuSettingsManager::setUIDockSize(const QString &dockName, const QSize &size)
{
    m_settings.setValue(UI_DOCK_SIZE+dockName, size);
}

QColor DuSettingsManager::uiIconColor(ColorVariant v) const
{
    QColor c = m_settings.value(UI_ICON_COLOR, QColor(177,177,177)).value<QColor>();
    if (v == DarkerColor) {
        int lightness = std::max(0, c.lightness() - 50);
        int saturation = std::max(0, c.hslSaturation() - 50);
        int hue = c.hslHue();
        c.setHsl(hue, saturation, lightness);
    }
    else if (v == LighterColor) {
        int lightness = std::min(255, c.lightness() + 50);
        int saturation = std::min(255, c.hslSaturation() + 50);
        int hue = c.hslHue();
        c.setHsl(hue, saturation, lightness);
    }
    return c;
}

void DuSettingsManager::setUIIconColor(const QColor &color)
{
    m_settings.setValue(UI_ICON_COLOR, color);
    emit uiIconColorChanged(color);
}

ColorVariant DuSettingsManager::trayIconMode() const
{
    return static_cast<ColorVariant>(
        m_settings.value(UI_TRAYICON_MODE, NormalColor).toInt()
        );
}

void DuSettingsManager::setTrayIconMode(ColorVariant mode)
{
    m_settings.setValue(UI_TRAYICON_MODE, mode);
    emit trayIconModeChanged(mode);
    emit trayIconColorChanged(trayIconColor());
}

QColor DuSettingsManager::trayIconColor() const
{
    ColorVariant mode = trayIconMode();
    switch (mode) {
    case NormalColor:
        return uiFocusColor();
    case DarkerColor:
        return QColor("#222222");
    case LighterColor:
        return QColor("#e3e3e3");
    }
    return uiFocusColor();
}

bool DuSettingsManager::showTrayIcon() const
{
    return m_settings.value(UI_SHOW_TRAYICON, false).toBool();
}

QString DuSettingsManager::uiDateFormat() const
{
    return m_settings.value(UI_DATE_FORMAT, "yyyy-MM-dd hh:mm:ss").toString();
}

bool DuSettingsManager::checkUpdates() const
{
    return m_settings.value(CHECK_UPDATES, true).toBool();
}

void DuSettingsManager::setShowTrayIcon(bool show)
{
    m_settings.setValue(UI_SHOW_TRAYICON, show);
    emit trayIconVisibilityChanged(show);
}

void DuSettingsManager::setUIDateFormat(const QString f)
{
    m_settings.setValue(UI_DATE_FORMAT, f);
    emit uiDateFormatChanged(f);
}

void DuSettingsManager::setCheckUpdates(bool c)
{
    m_settings.setValue(CHECK_UPDATES, c);
}

int DuSettingsManager::daemonPort() const {
    return m_settings.value(DAEMON_PORT, 18185).toInt();
}

void DuSettingsManager::setDaemonPort(int p) {
    m_settings.setValue(DAEMON_PORT, p);
}

DuSettingsManager::DuSettingsManager(QObject *parent)
    : QObject{parent}
{

}
