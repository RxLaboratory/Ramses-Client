#ifndef DUSETTINGSMANAGER_H
#define DUSETTINGSMANAGER_H

#include <QObject>
#include <QSettings>
#include <QColor>

#include "enums.h"

class DuSettingsManager : public QObject
{
    Q_OBJECT
public:
    // Settings keys
    static const QString UI_FOCUS_COLOR;
    static const QString UI_TOOLBUTTON_STYLE;
    static const QString NV_CURVATURE;
    static const QString UI_WINDOW_GEOMETRY;
    static const QString UI_WINDOW_STATE;
    static const QString UI_DOCK_SIZE;
    static const QString UI_ICON_COLOR;
    static const QString UI_TRAYICON_MODE;
    static const QString UI_SHOW_TRAYICON;
    static const QString UI_DATE_FORMAT;
    static const QString CHECK_UPDATES;
    static const QString DAEMON_PORT;

    static DuSettingsManager *instance();

    QColor uiFocusColor(ColorVariant v=NormalColor) const;
    /**
     * @brief The curvature of the connections in the node view
     * @param A ratio in [0.0, 1.0]
     */
    float nvCurvature() const;

    /**
     * @brief The geometry of the top level window
     * @return
     */
    QByteArray uiWindowGeometry() const;
    /**
     * @brief The state of the main window and all its children
     * @return
     */
    QByteArray uiWindowState() const;

    Qt::ToolButtonStyle uiToolButtonStyle() const;

    QSize uiDockSize(const QString &dockName) const;

    QColor uiIconColor(ColorVariant v=NormalColor) const;
    QColor trayIconColor() const;
    ColorVariant trayIconMode() const;
    bool showTrayIcon() const;
    QString uiDateFormat() const;
    bool checkUpdates() const;
    int daemonPort() const;

public slots:
    void setUIFocusColor(const QColor &color);
    /**
     * @brief Sets the curvature of the connections in the node view
     * @param Either a float < 1 or an int in [0,100]
     */
    void setNVCurvature(float c);

    /**
     * @brief Saves the window geometry and state to restore it on next launch
     * @param geometry
     * @param state
     */
    void saveUIWindowState(const QByteArray &geometry, const QByteArray &state);

    void setUIToolButtonStyle(Qt::ToolButtonStyle style);
    void setUIToolButtonStyle(int style);
    void setUIDockSize(const QString &dockName, const QSize &size);
    void setUIIconColor(const QColor &color);
    void setTrayIconMode(ColorVariant mode);
    void setShowTrayIcon(bool show);
    void setUIDateFormat(const QString f);
    void setCheckUpdates(bool c);
    void setDaemonPort(int p);

signals:
    void nvCurvatureChanged(float);
    void uiToolButtonStyleChanged(Qt::ToolButtonStyle);
    void uiFocusColorChanged(QColor);
    void uiIconColorChanged(QColor);
    void trayIconModeChanged(ColorVariant);
    void trayIconColorChanged(QColor);
    void trayIconVisibilityChanged(bool);
    void uiDateFormatChanged(QString);

protected:
    static DuSettingsManager *_instance;
    // Singleton
    explicit DuSettingsManager(QObject *parent = nullptr);

private:
    QSettings m_settings;
};

#endif // DUSETTINGSMANAGER_H
