#ifndef DUSETTINGS_H
#define DUSETTINGS_H

#include "qdatetime.h"
#include <QObject>
#include <QSettings>
#include <QColor>
#include <QSize>

class DuSettings: public QObject
{
    Q_OBJECT

public: // enums and structs

    enum SettingKey {
        UI_FocusColor,
        UI_IconColor,
        UI_BackgroundColor,
        UI_ForegroundColor,
        UI_Contrast,
        UI_UIFont,
        UI_ContentFont,
        UI_Margins,
        UI_WindowGeometry,
        UI_WindowState,
        UI_ToolButtonStyle,
        UI_DockSize,
        UI_NodeViewCurvature,
        UI_DateFormat,
        UI_TimeFormat,
        UI_DateTimeFormat,
        UI_ShowTrayIcon,
        UI_TrayIconMode,
        APP_Version,
        APP_CheckUpdates,
        APP_LastUpdateCheck,
        Other,
    };

    struct DuSetting {
        QString qKey;
        QVariant defaultValue;
    };

private: // The list of settings

    QHash<int, DuSetting> _settings = {
        { UI_FocusColor, { "ui/focusColor", QColor(165,38,196) } }, // RxLab Purple
        { UI_IconColor, { "ui/iconColor", QColor(177,177,177) } },
        { UI_BackgroundColor, {"ui/backgroundColor", QColor(51,51,51) }},
        { UI_ForegroundColor, {"ui/foregroundColor", QColor(177,177,177) }},
        { UI_Contrast, {"ui/contrast", 1}},
        { UI_UIFont, {"ui/uiFont", "Nunito" }},
        { UI_ContentFont, {"ui/contentFont", "Ibarra Real Nova" }},
        { UI_Margins, {"ui/margins", 3 }},
        { UI_WindowGeometry, { "ui/windowGeometry", QByteArray() }},
        { UI_WindowState, { "ui/windowState", QByteArray() }},
        { UI_ToolButtonStyle, { "ui/toolButtonStyle", Qt::ToolButtonIconOnly }},
        { UI_DockSize, { "ui/dockSize/", QSize(300, 300) }},
        { UI_NodeViewCurvature, { "ui/nodeViewCurvature", 0.333 }},
        { UI_DateFormat, { "ui/dateFormat", "yyyy/MM/dd" }},
        { UI_TimeFormat, { "ui/timeFormat", "hh:mm:ss"}},
        { UI_DateTimeFormat, { "ui/dateTimeFormat", "yyyy/MM/dd hh:mm:ss"}},
        { UI_ShowTrayIcon, { "ui/showTrayIcon", false}},
        { UI_TrayIconMode, { "ui/trayIconMode", "color"}},
        { APP_Version, { "app/version", "0.0.0"}},
        { APP_CheckUpdates, { "app/checkUpdates", true}},
        { APP_LastUpdateCheck, {"app/latestUpdateCheck" , QDateTime(QDate(1818,05,05), QTime())}},
    };

public: // Methods

    /**
     * @brief Builds the DuSettings single instance
     * @return The single instance
     */
    static DuSettings *i();

    /**
     * @brief Get a setting.
     * @param defaultOverride Overrides the default default value ;)
     * @return In this order: The value, or the default override, or the setting default value.
     */
    QVariant get(int key, const QVariant &defaultOverride = QVariant(), const QString &subKey = "") const;
    /**
     * @brief Set a setting.
     * @param value The value to set.
     * @param doNotEmit Set this to true to prevent the settingChanged signal to be emitted.
     */
    void set(int key, const QVariant &value, const QString &subKey = "", bool doNotEmit = false);

    void registerSettings(const QHash<int, DuSetting> &settings);

signals:
    void settingChanged(int,const QVariant &);

private: // Internal stuff

    // The actual settings managed by Qt
    QSettings _s;
    // The single instance
    static DuSettings *_instance;
    // This is a singleton
    explicit DuSettings(QObject *parent = nullptr);
};

#endif // DUSETTINGSMANAGER_H
