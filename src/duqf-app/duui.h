#ifndef DUUI_H
#define DUUI_H

#include <QString>
#include <QWidget>
#include <QCommandLineParser>

class DuSplashScreen;

/**
 * @brief The DuUI class
 * @version 1.1.0
 */
class DuUI
{
public:
    static void buildUI(const QCommandLineParser &cli, DuSplashScreen *s);
    /**
     * @brief Gets a stylesheet for a specific css class
     * @param cssClass The name of the class
     * The classe must be found in ":/styles/className"
     * If it's an empty string, gets and sets the default css for widgets without class,
     * which is ":/styles/default"
     * @return the style sheets
     */
    static QString css(const QString &cssClass = "default", bool loadSettings = true);
    static void setCSS(QWidget *w, const QString &css, bool loadSettings = true);
    static void addCSS(QWidget *w, const QString &cssClass, bool loadSettings = true);
    static void addCustomCSS(QWidget *w, const QString &customCSS, bool loadSettings = true);
    static void replaceCSS(QWidget *w, const QString &css, const QString &where);
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
    /**
     * @brief Sets the application tool buttons style
     * @param styleIndex
     */
    static void setAppToolButtonStyle(Qt::ToolButtonStyle style);
    /**
     * @brief Sets the font for the whole app
     * @param family
     * @param size
     * @param weight
     */
    static void setFont(QString family = "Ubuntu", int size=10, int weight=400);

    static qreal desktopScale();
    static int adjustToDpi(int px);
    static qreal adjustToDpi(qreal px);
    static QSize adjustToDpi(const QSize &s);

    static void setupLayout(QLayout *l, int margin, int spacing = -1);


protected:
    const static QRegularExpression _cssVarsRE;
    const static QRegularExpression _cssPxValRE;
    static QHash<QString,QString> _cssVars;
    static qreal _desktopScale;

    DuUI();
};

#endif // DUUI_H
