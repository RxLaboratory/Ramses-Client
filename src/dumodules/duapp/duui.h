#ifndef DUUI_H
#define DUUI_H

#include <QCommandLineParser>
#include <QStackedLayout>
#include <QMainWindow>
#include <QFormLayout>

#ifdef Q_OS_WIN
#include "dwmapi.h"
#endif

class DuSplashScreen;

/**
 * @brief The DuUI class provides UI Tools
 * @version 1.1.1
 * Add addStackedLayout() methods
 * 1.1.0
 * Add Layout methods & toolbutton style
 */
class DuUI
{
public:

    // BUILD UI

    /**
     * @brief Set the correct flags for the app to be scaled on HiDPI/Retina screens
     * This function must be called before the UI is created, at the beginning of main()
     */
    static void setUseHDPI();
    /**
     * @brief Create and show a MainWindow instance.
     * @param cli The command line parser which will be passed to the MainWindow.
     * @param s The splash screen used during main() which will be hidden when the MainWindow is ready.
     */
    static void buildUI(const QCommandLineParser &cli, DuSplashScreen *s);

    // STYLESHEETS

    /**
     * @brief Gets a stylesheet for a specific css class
     * @param cssClass The name of the class
     * The classe must be found in ":/styles/className"
     * If it's an empty string, gets and sets the default css for widgets without class,
     * which is ":/styles/default"
     * @return the style sheets
     */
    static QString css(const QString &cssClass = "", bool loadSettings = true);
    static void setCSS(QWidget *w, const QString &css, bool loadSettings = true);
    static void addCSS(QWidget *w, const QString &cssClass, bool loadSettings = true);
    static void addCustomCSS(QWidget *w, const QString &customCSS, bool loadSettings = true);
    static QString setCSSVars(const QString &css, bool loadSettings = true);
    /**
     * @brief The list of variables used in css in the form { "@key": "value" }.
     * The vars file must be ":/styles/vars"
     * @return The list.
     */
    static QHash<QString,QString> cssVars(bool loadSettings = true);
    /**
     * @brief Sets the application stylesheet
     * @param css
     */
    static void setAppCss(QString css);

    // FONTS

    /**
     * @brief Adds all fonts from :/fonts/ to the application
     */
    static void addApplicationFonts();
    /**
     * @brief Sets the font for the whole app
     * @param family
     * @param size
     * @param weight
     */
    static void setFont(QString family = "Ubuntu", int size=10, int weight=400);

    // THEMES

    static bool negatedTheme();
    static void setDarkTitleBar(QWidget *widget);
    static QColor pullColor(const QColor &color, qreal q = 1.0);
    static QColor pushColor(const QColor &color, qreal q = 1.0);
    static QColor toForegroundValue(const QColor &color);

    // OTHER STYLE OPTIONS

    /**
     * @brief Sets the application tool buttons style
     * @param styleIndex
     */
    static void setAppToolButtonStyle(Qt::ToolButtonStyle style);

    // UTILS

    static QMainWindow *appMainWindow();

    // UI Creation

    static void setupLayout(QLayout *layout, bool isSubLayout = false);

    static QBoxLayout *createBoxLayout(Qt::Orientation orientation = Qt::Vertical, bool isSubLayout = true, QWidget *parent = nullptr);
    static QBoxLayout *addBoxLayout(Qt::Orientation orientation, QWidget *parent);
    static QBoxLayout *addBoxLayout(Qt::Orientation orientation, QBoxLayout *parent);
    static QBoxLayout *addBoxLayout(Qt::Orientation orientation, QGridLayout *parent, int row, int column);
    static QBoxLayout *addBoxLayout(Qt::Orientation orientation, QFormLayout *parent, const QString label = "");

    static QFormLayout *createFormLayout(bool isSubLayout = true, QWidget *parent = nullptr);
    static QFormLayout *addFormLayout(QWidget *parent);
    static QFormLayout *addFormLayout(QBoxLayout *parent);

    static QGridLayout *createGridLayout(bool isSubLayout = true, QWidget *parent = nullptr);
    static QGridLayout *addGridLayout(QWidget *parent);
    static QGridLayout *addGridLayout(QBoxLayout *parent);

    static QStackedLayout *createStackedLayout(bool isSubLayout = true, QWidget *parent = nullptr);
    static QStackedLayout *addStackedLayout(QWidget *parent);
    static QStackedLayout *addStackedLayout(QBoxLayout *parent);

    static void centerLayout(QLayout *child, QWidget *parent);

    static QWidget *addBlock(QLayout *child, QBoxLayout *parent);

protected:
    const static QRegularExpression _cssVarsRE;
    const static QRegularExpression _cssPxValRE;
    static QHash<QString,QString> _cssVars;
    static qreal _desktopScale;

    DuUI();

private:
#ifdef Q_OS_WIN
    enum PreferredAppMode {
        Default,
        AllowDark,
        ForceDark,
        ForceLight,
        Max
    };

    enum WINDOWCOMPOSITIONATTRIB {
        WCA_UNDEFINED = 0,
        WCA_NCRENDERING_ENABLED = 1,
        WCA_NCRENDERING_POLICY = 2,
        WCA_TRANSITIONS_FORCEDISABLED = 3,
        WCA_ALLOW_NCPAINT = 4,
        WCA_CAPTION_BUTTON_BOUNDS = 5,
        WCA_NONCLIENT_RTL_LAYOUT = 6,
        WCA_FORCE_ICONIC_REPRESENTATION = 7,
        WCA_EXTENDED_FRAME_BOUNDS = 8,
        WCA_HAS_ICONIC_BITMAP = 9,
        WCA_THEME_ATTRIBUTES = 10,
        WCA_NCRENDERING_EXILED = 11,
        WCA_NCADORNMENTINFO = 12,
        WCA_EXCLUDED_FROM_LIVEPREVIEW = 13,
        WCA_VIDEO_OVERLAY_ACTIVE = 14,
        WCA_FORCE_ACTIVEWINDOW_APPEARANCE = 15,
        WCA_DISALLOW_PEEK = 16,
        WCA_CLOAK = 17,
        WCA_CLOAKED = 18,
        WCA_ACCENT_POLICY = 19,
        WCA_FREEZE_REPRESENTATION = 20,
        WCA_EVER_UNCLOAKED = 21,
        WCA_VISUAL_OWNER = 22,
        WCA_HOLOGRAPHIC = 23,
        WCA_EXCLUDED_FROM_DDA = 24,
        WCA_PASSIVEUPDATEMODE = 25,
        WCA_USEDARKMODECOLORS = 26,
        WCA_LAST = 27
    };

    struct WINDOWCOMPOSITIONATTRIBDATA {
        WINDOWCOMPOSITIONATTRIB Attrib;
        PVOID pvData;
        SIZE_T cbData;
    };

    using fnAllowDarkModeForWindow =  BOOL (WINAPI *)(HWND hWnd, BOOL allow);
    using fnSetPreferredAppMode = PreferredAppMode (WINAPI *)(PreferredAppMode appMode);
    using fnSetWindowCompositionAttribute =  BOOL (WINAPI *)(HWND hwnd, WINDOWCOMPOSITIONATTRIBDATA *);
#endif
};

#endif // DUUI_H
