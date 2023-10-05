#ifndef DUSETTINGSMANAGER_H
#define DUSETTINGSMANAGER_H


class DuSettingsManager : public QObject
{
    Q_OBJECT
public:
    // Settings keys
    static const QString UI_FOCUS_COLOR;
    static const QString NV_CURVATURE;
    static const QString UI_WINDOW_GEOMETRY;
    static const QString UI_WINDOW_STATE;

    // Colors
    enum ColorVariant {
        NormalColor,
        DarkerColor,
        LighterColor
    };

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

signals:
    void nvCurvatureChanged(float);

protected:
    static DuSettingsManager *_instance;
    // Singleton
    explicit DuSettingsManager(QObject *parent = nullptr);

private:
    QSettings m_settings;
};

#endif // DUSETTINGSMANAGER_H
