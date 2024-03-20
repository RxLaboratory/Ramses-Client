#include "duui.h"

#include <QRegularExpression>
#include <QFile>
#include <QMainWindow>
#include <QStyleFactory>
#include <QApplication>
#include <QToolBar>
#include <QToolButton>
#include <QFontDatabase>
#include <QDesktopWidget>
#include <QtDebug>
#include <QLayout>

#include "duqf-app/dusettingsmanager.h"
#include "duqf-app/dustyle.h"
#include "duqf-widgets/dusplashscreen.h"
#include "mainwindow.h"

QHash<QString,QString> DuUI::_cssVars = QHash<QString,QString>();
const QRegularExpression DuUI::_cssVarsRE = QRegularExpression("(@[a-zA-Z0-9-]+)\\s*=\\s*(\\S+)");
const QRegularExpression DuUI::_cssPxValRE = QRegularExpression("(\\d+)px");
qreal DuUI::_desktopScale = -1;

DuUI::DuUI()
{

}

void DuUI::buildUI(const QCommandLineParser &cli, DuSplashScreen *s)
{
    s->newMessage("Building UI");

    MainWindow *w = new MainWindow( cli );

    // Restore Geometry and state
    w->restoreGeometry( DuSettingsManager::instance()->uiWindowGeometry() );
    w->restoreState(DuSettingsManager::instance()->uiWindowState());
    // But hide the docks for a cleaner look
    //w->hideAllDocks();

    //if (!cli.isSet("no_ui"))
    w->show();

    //hide splash when finished
    s->finish(w);
}

QString DuUI::css(const QString &cssClass, bool loadSettings)
{
    QString c = "";
    QFile f(":/styles/" + cssClass);
    if (f.exists() && f.open(QFile::ReadOnly)) {
        c = f.readAll();
        f.close();
    }

    // Replace variables
    auto i = QHashIterator<QString,QString>(cssVars(loadSettings));
    while(i.hasNext()) {
        i.next();
        QRegularExpression re( i.key() + "([,);\\s\\r\\n])" );
        c.replace(re, i.value() + "\\1");
    }

    return c;
}

void DuUI::setCSS(QWidget *w, const QString &css, bool loadSettings)
{
    w->setStyleSheet(
        setCSSVars(css, loadSettings)
        );
}

void DuUI::addCSS(QWidget *w, const QString &cssClass, bool loadSettings)
{
    QString css = w->styleSheet();
    w->setStyleSheet(css + "\n" + DuUI::css(cssClass, loadSettings));
}

void DuUI::addCustomCSS(QWidget *w, const QString &customCSS, bool loadSettings)
{
    setCSSVars(customCSS, loadSettings);
    w->setStyleSheet(
        w->styleSheet() + "\n" + customCSS
        );
}

void DuUI::replaceCSS(QWidget *w, const QString &css, const QString &where)
{
    const QStringList sList = w->styleSheet().split("\n");
    QStringList newStyleList;
    bool in = false;
    bool replaced = false;
    for (const QString &s: sList) {
        if (!in)
            newStyleList << s;

        if (s == "/*DuUI:"+where+"*/") {
            newStyleList << css;
            replaced = true;
            in = true;
        }

        if (s == "/*DuUI:End:"+where+"*/") {
            newStyleList << s;
            in = false;
        }
    }
    if (!replaced) {
        newStyleList << "/*DuUI:"+where+"*/";
        newStyleList << css;
        newStyleList << "/*DuUI:End:"+where+"*/";
    }

    w->setStyleSheet(newStyleList.join("\n"));
}

QString DuUI::setCSSVars(const QString &css, bool loadSettings)
{
    QString s = css;
    // Replace variables
    auto i = QHashIterator<QString,QString>(cssVars(loadSettings));
    while(i.hasNext()) {
        i.next();
        QRegularExpression re( i.key() + "([,);\\s\\r\\n])" );
        s.replace(re, i.value() + "\\1");
    }
    return s;
}

QHash<QString, QString> DuUI::cssVars(bool loadSettings)
{
    if (!_cssVars.isEmpty()) return _cssVars;

    // Insert runtime vars
    if (loadSettings) {
        _cssVars.insert(
            "@focus-dark",
            DuSettingsManager::instance()->uiFocusColor(DarkerColor).name()
            );
        _cssVars.insert(
            "@focus",
            DuSettingsManager::instance()->uiFocusColor().name()
            );
        _cssVars.insert(
            "@focus-light",
            DuSettingsManager::instance()->uiFocusColor(LighterColor).name()
            );
    }

    QFile varsFile(":/styles/vars");
    if (varsFile.exists() && varsFile.open(QFile::ReadOnly)) {
        //read lines
        while(!varsFile.atEnd())
        {
            QString line = varsFile.readLine();

            qreal s = desktopScale();

            //get value and name
            QRegularExpressionMatch match = _cssVarsRE.match(line);
            if (match.hasMatch())
            {
                QString key = match.captured(1);
                QString val = match.captured(2);
                if (val.endsWith("px") && s != 1) {
                    // Auto Scale pixels
                    const int v = val.leftRef(val.indexOf("px")).toInt();
                    val = QString::number( v*s ) + "px";
                }
                _cssVars.insert(key, val);
            }
        }
    }

    return _cssVars;
}

void DuUI::setAppCss(QString css)
{
    // Remove
    qApp->setStyleSheet("");

    // Try to set the style always from the same base, Windows
    // (Fusion has many issues)
    // To make sure we use the same style on all platforms
    QStyle *s = nullptr;
    QStringList k = QStyleFactory::keys();
    if (k.contains("windows")) s = new DuStyle("windows");
    else if (k.contains("Windows")) s = new DuStyle("windows");
    qApp->setStyle(s);
    // Apply our tweaks to the palette
    QPalette palette;
    palette.setColor(QPalette::Active, QPalette::Highlight, DuSettingsManager::instance()->uiFocusColor());
    qApp->setPalette(palette);

    // Reset
    qApp->setStyleSheet(css);
}

void DuUI::setAppToolButtonStyle(Qt::ToolButtonStyle style)
{
    foreach( QWidget *w, qApp->allWidgets())
    {
        if(QMainWindow *mw = qobject_cast<QMainWindow*>(w))
        {
            mw->setToolButtonStyle(style);
        }
        else if (QToolBar *tb = qobject_cast<QToolBar*>(w))
        {
            tb->setToolButtonStyle(style);
        }
        else if (QToolButton *tb = qobject_cast<QToolButton*>(w))
        {
            if (tb->objectName() == "windowButton") continue;
            if (tb->objectName() == "menuButton") continue;
            if (tb->text() == "") continue;
            if (tb->icon().isNull()) continue;
            tb->setToolButtonStyle(style);
        }
    }
}

void DuUI::setFont(QString family, int size, int weight)
{
    Q_UNUSED(size);
    Q_UNUSED(weight);
    if (family == "Ubuntu")
    {
        //add fonts
        QFontDatabase::addApplicationFont(":/fonts/Ubuntu-B");
        QFontDatabase::addApplicationFont(":/fonts/Ubuntu-BI");
        QFontDatabase::addApplicationFont(":/fonts/Ubuntu-C");
        QFontDatabase::addApplicationFont(":/fonts/Ubuntu-L");
        QFontDatabase::addApplicationFont(":/fonts/Ubuntu-LI");
        QFontDatabase::addApplicationFont(":/fonts/Ubuntu-M");
        QFontDatabase::addApplicationFont(":/fonts/Ubuntu-MI");
        QFontDatabase::addApplicationFont(":/fonts/Ubuntu-R");
        QFontDatabase::addApplicationFont(":/fonts/Ubuntu-RI");
        QFontDatabase::addApplicationFont(":/fonts/Ubuntu-Th");
        QFontDatabase::addApplicationFont(":/fonts/UbuntuMono-B");
        QFontDatabase::addApplicationFont(":/fonts/UbuntuMono-BI");
        QFontDatabase::addApplicationFont(":/fonts/UbuntuMono-R");
        QFontDatabase::addApplicationFont(":/fonts/UbuntuMono-RI");
    }

    qApp->setFont(QFont(family),"QWidget");
}

qreal DuUI::desktopScale()
{
    // Get only the first time, kept as a static var
    if (_desktopScale > 0) return _desktopScale;

    qreal dpi = qApp->desktop()->logicalDpiX();
#ifdef QT_DEBUG
    qDebug().noquote() << "Desktop DPI: " << dpi;
#endif
    _desktopScale = dpi/96.0;
    return _desktopScale;
}

int DuUI::adjustToDpi(int px)
{
    return px * desktopScale();
}

qreal DuUI::adjustToDpi(qreal px)
{
    return px * desktopScale();
}

QSize DuUI::adjustToDpi(const QSize &s)
{
    return QSize(
        adjustToDpi( s.width() ),
        adjustToDpi( s.height() )
        );
}

void DuUI::setupLayout(QLayout *l, int margin, int spacing)
{
    if (spacing == -1) spacing = margin;
    margin = adjustToDpi(margin);
    l->setContentsMargins( margin,margin,margin,margin );
    l->setSpacing(adjustToDpi(spacing));
}

void DuUI::setDarkTitleBar(QWidget *widget)
{
    auto hwnd = reinterpret_cast<HWND>(widget->winId());
#ifdef Q_OS_WIN
    HMODULE hUxtheme = LoadLibraryExW(L"uxtheme.dll", NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);
    HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
    fnAllowDarkModeForWindow AllowDarkModeForWindow
        = reinterpret_cast<fnAllowDarkModeForWindow>(GetProcAddress(hUxtheme, MAKEINTRESOURCEA(133)));
    fnSetPreferredAppMode SetPreferredAppMode
        = reinterpret_cast<fnSetPreferredAppMode>(GetProcAddress(hUxtheme, MAKEINTRESOURCEA(135)));
    fnSetWindowCompositionAttribute SetWindowCompositionAttribute
        = reinterpret_cast<fnSetWindowCompositionAttribute>(GetProcAddress(hUser32, "SetWindowCompositionAttribute"));

    SetPreferredAppMode(AllowDark);
    BOOL dark = TRUE;
    AllowDarkModeForWindow(hwnd, dark);
    WINDOWCOMPOSITIONATTRIBDATA data = {
        WCA_USEDARKMODECOLORS,
        &dark,
        sizeof(dark)
    };
    SetWindowCompositionAttribute(hwnd, &data);
#endif
}
