#ifndef DUSETTINGSMANAGER_H
#define DUSETTINGSMANAGER_H


class DuSettingsManager : public QObject
{
    Q_OBJECT
public:
    // Settings keys
    static const QString FOCUS_COLOR;
    static const QString NODESVIEW_CURVATURE;

    // Colors
    enum ColorVariant {
        NormalColor,
        DarkerColor,
        LighterColor
    };

    static DuSettingsManager *instance();

    QColor focusColor(ColorVariant v=NormalColor) const;
    /**
     * @brief The curvature of the connections in the node view
     * @param A ratio in [0.0, 1.0]
     */
    float nodesViewCurvature() const;

public slots:
    void setFocusColor(const QColor &color);
    /**
     * @brief Sets the curvature of the connections in the node view
     * @param Either a float < 1 or an int in [0,100]
     */
    void setNodesViewCurvature(float c);

signals:
    void nodesViewCurvatureChanged(float);

protected:
    static DuSettingsManager *_instance;
    // Singleton
    explicit DuSettingsManager(QObject *parent = nullptr);

private:
    QSettings m_settings;
};

#endif // DUSETTINGSMANAGER_H
