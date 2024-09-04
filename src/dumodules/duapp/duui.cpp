#include "duui.h"
#include "mainwindow.h"
#include "duapp/app-utils.h"
#include "dustyle.h"
#include "duapp/dusettings.h"
#include "duutils/ducolorutils.h"

QHash<QString,QString> DuUI::_cssVars = QHash<QString,QString>();
const QRegularExpression DuUI::_cssVarsRE = QRegularExpression("(@[a-zA-Z0-9-]+)\\s*=\\s*(\\S+)");
const QRegularExpression DuUI::_cssPxValRE = QRegularExpression("(\\d+)px");
qreal DuUI::_desktopScale = -1;
MainWindow *DuUI::_mw = nullptr;

DuUI::DuUI()
{

}

void DuUI::setUseHDPI()
{
    // Not needed on Qt 6
#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0) && defined(Q_OS_WIN)
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
#endif
}

void DuUI::buildUI(const QCommandLineParser &cli, DuSplashScreen *s)
{
    s->newMessage("Building UI");

    _mw = new MainWindow( cli );

    // Restore Geometry and state
    _mw->restoreGeometry( DuSettings::i()->get(DuSettings::UI_WindowGeometry).toByteArray() );
    _mw->restoreState( DuSettings::i()->get(DuSettings::UI_WindowState).toByteArray() );
    // But hide the docks for a cleaner look
    _mw->hideAllDocks();

    //if (!cli.isSet("no_ui"))
    _mw->show();

    //hide splash when finished
    s->finish(_mw);
}

QString DuUI::css(const QString &cssClass, bool loadSettings)
{
    QString c = "";

    // Concat all stylesheets
    QDir d(":/styles/");
    const QStringList styles = d.entryList(QDir::Files, QDir::Name); // The order is important!
    for(const auto &style: styles) {
        if (style == "vars")
            continue;
        if (cssClass != ""  && !style.contains(cssClass))
            continue;

        QFile f(":/styles/"+style);
        if (f.open(QFile::ReadOnly)) {
            c += "\n" + f.readAll();
            f.close();
        }
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
    QString css = setCSSVars(customCSS, loadSettings);
    w->setStyleSheet(
        w->styleSheet() + "\n" + css
        );
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

    QColor focus = DuSettings::i()->get(DuSettings::UI_FocusColor).value<QColor>();
    QColor bg = DuSettings::i()->get(DuSettings::UI_BackgroundColor).value<QColor>();
    QColor fg = DuSettings::i()->get(DuSettings::UI_ForegroundColor).value<QColor>();
    int margin = DuSettings::i()->get(DuSettings::UI_Margins).toInt();

    // Insert runtime vars
    if (loadSettings) {
        _cssVars.insert(
            "@margin",
            QString::number(margin) + "px"
            );
        _cssVars.insert(
            "@margin-small",
            QString::number(margin/2) + "px"
            );
        _cssVars.insert(
            "@margin-big",
            QString::number(margin*2) + "px"
            );
        _cssVars.insert(
            "@radius",
            QString::number(margin*2) + "px"
            );
        _cssVars.insert(
            "@radius-big",
            QString::number(margin*3) + "px"
            );
        _cssVars.insert(
            "@focus-pull",
            pullColor(focus).name()
            );
        _cssVars.insert(
            "@focus-puller",
            pullColor(focus, 2).name()
            );
        _cssVars.insert(
            "@focus",
            focus.name()
            );
        _cssVars.insert(
            "@focus-push",
            pushColor(focus).name()
            );
        _cssVars.insert(
            "@focus-push-more",
            pushColor(focus, 2).name()
            );
        _cssVars.insert(
            "@background",
            bg.name()
            );
        _cssVars.insert(
            "@background-pull",
            pullColor(bg).name()
            );
        _cssVars.insert(
            "@background-pull-more",
            pullColor(bg, 2).name()
            );
        _cssVars.insert(
            "@background-push",
            pushColor(bg).name()
            );
        _cssVars.insert(
            "@background-push-more",
            pushColor(bg, 2).name()
            );
        _cssVars.insert(
            "@foreground",
            fg.name()
            );
        _cssVars.insert(
            "@foreground-pull",
            pullColor(fg).name()
            );
        _cssVars.insert(
            "@foreground-pull-more",
            pullColor(fg, 2).name()
            );
        _cssVars.insert(
            "@foreground-push",
            pushColor(fg).name()
            );
        _cssVars.insert(
            "@foreground-push-more",
            pushColor(fg, 2).name()
            );
        _cssVars.insert(
            "@ui-font",
            DuSettings::i()->get(DuSettings::UI_UIFont).toString()
            );
        _cssVars.insert(
            "@content-font",
            DuSettings::i()->get(DuSettings::UI_ContentFont).toString()
            );
    }

    QFile varsFile(":/styles/vars");
    if (varsFile.exists() && varsFile.open(QFile::ReadOnly)) {
        //read lines
        while(!varsFile.atEnd())
        {
            QString line = varsFile.readLine();
            //get value and name
            QRegularExpressionMatch match = _cssVarsRE.match(line);
            if (match.hasMatch())
            {
                QString key = match.captured(1);
                QString val = match.captured(2);
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
    palette.setColor(QPalette::Active, QPalette::Highlight, DuSettings::i()->get(DuSettings::UI_FocusColor).value<QColor>());
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

void DuUI::addApplicationFonts()
{
    QDir d(":/fonts");
    const QStringList fontFiles = d.entryList(QDir::Files);
    for (const auto &fontFile: fontFiles) {
        QFontDatabase::addApplicationFont(":/fonts/"+fontFile);
    }
}

void DuUI::setFont(QString family, int size, int weight)
{
    Q_UNUSED(size);
    Q_UNUSED(weight);
    qApp->setFont(QFont(family),"QWidget");
}

bool DuUI::negatedTheme()
{
    auto bg = DuSettings::i()->get(DuSettings::UI_BackgroundColor).value<QColor>();
    auto fg = DuSettings::i()->get(DuSettings::UI_ForegroundColor).value<QColor>();

    return fg.lightness() > bg.lightness();
}

void DuUI::setDarkTitleBar(QWidget *widget)
{
#ifdef Q_OS_WIN
    auto hwnd = reinterpret_cast<HWND>(widget->winId());
    HMODULE hUxtheme = LoadLibraryExW(L"uxtheme.dll", NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);
    HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-function-type"
    // clang-format off
    // Don't touch this!
    fnAllowDarkModeForWindow AllowDarkModeForWindow
        = reinterpret_cast<fnAllowDarkModeForWindow>(GetProcAddress(hUxtheme, MAKEINTRESOURCEA(133)));
    fnSetPreferredAppMode SetPreferredAppMode
        = reinterpret_cast<fnSetPreferredAppMode>(GetProcAddress(hUxtheme, MAKEINTRESOURCEA(135)));
    fnSetWindowCompositionAttribute SetWindowCompositionAttribute
        = reinterpret_cast<fnSetWindowCompositionAttribute>(GetProcAddress(hUser32, "SetWindowCompositionAttribute"));
    // clang-format on
#pragma GCC diagnostic pop

    SetPreferredAppMode(AllowDark);
    BOOL dark = TRUE;
    AllowDarkModeForWindow(hwnd, dark);
    WINDOWCOMPOSITIONATTRIBDATA data = {
        WCA_USEDARKMODECOLORS,
        &dark,
        sizeof(dark)
    };
    SetWindowCompositionAttribute(hwnd, &data);
#else
    Q_UNUSED(widget)
#endif
}

QColor DuUI::pullColor(const QColor &color, qreal q)
{
    int contrast = DuSettings::i()->get(
                                      DuSettings::UI_Contrast
                                      ).toInt();
    // The foreground is lighter
    if (negatedTheme())
        return DuColorUtils::lighter(
            color,
            100 + 20*contrast*q
            );

    return DuColorUtils::darker(
        color,
        100 + 20*contrast*q
        );
}

QColor DuUI::pushColor(const QColor &color, qreal q)
{
    int contrast = DuSettings::i()->get(
                                      DuSettings::UI_Contrast
                                      ).toInt();
    // The background is darker
    if (negatedTheme())
        return DuColorUtils::darker(
            color,
            100 + 20*contrast*q
            );

    return DuColorUtils::lighter(
        color,
        100 + 20*contrast*q
        );
}

QColor DuUI::toForegroundValue(const QColor &color)
{
    QColor fgCol = DuSettings::i()->get(
                                      DuSettings::UI_ForegroundColor
                                      ).value<QColor>();
    return QColor::fromHsv(color.hue(), color.saturation(), fgCol.value());
}

MainWindow *DuUI::appMainWindow()
{
    return _mw;
}

void DuUI::setupLayout(QLayout *layout, bool isSubLayout)
{
    int m = DuSettings::i()->get(DuSettings::UI_Margins).toInt();
    if (isSubLayout) layout->setContentsMargins(0,0,0,0);
    else layout->setContentsMargins(m,m,m,m);
    layout->setSpacing(m);
}

QBoxLayout *DuUI::createBoxLayout(Qt::Orientation orientation, bool isSubLayout, QWidget *parent)
{
    QBoxLayout *layout;

    switch(orientation) {
    case Qt::Horizontal:
        layout = new QHBoxLayout(parent);
        break;
    case Qt::Vertical:
        layout = new QVBoxLayout(parent);
        break;
    }

    setupLayout(layout, isSubLayout);

    return layout;
}

QBoxLayout *DuUI::addBoxLayout(Qt::Orientation orientation, QWidget *parent)
{
    return createBoxLayout(orientation, false, parent);
}

QBoxLayout *DuUI::addBoxLayout(Qt::Orientation orientation, QBoxLayout *parent)
{
    QBoxLayout *layout = createBoxLayout(orientation, true);
    parent->addLayout(layout);
    return layout;
}

QBoxLayout *DuUI::addBoxLayout(Qt::Orientation orientation, QGridLayout *parent, int row, int column)
{
    QBoxLayout *layout = createBoxLayout(orientation, true);
    parent->addLayout(layout, row, column);
    return layout;
}

QBoxLayout *DuUI::addBoxLayout(Qt::Orientation orientation, QFormLayout *parent, const QString label)
{
    QBoxLayout *layout = createBoxLayout(orientation, true);
    if (label == "") parent->addRow(layout);
    else parent->addRow(label, layout);
    return layout;
}

QFormLayout *DuUI::createFormLayout(bool isSubLayout, QWidget *parent)
{
    QFormLayout *layout = new QFormLayout(parent);
    setupLayout(layout, isSubLayout);
    return layout;
}

QFormLayout *DuUI::addFormLayout(QWidget *parent)
{
    return createFormLayout(false, parent);
}

QFormLayout *DuUI::addFormLayout(QBoxLayout *parent)
{
    QFormLayout *layout = createFormLayout(true);
    parent->addLayout(layout);
    return layout;
}

QGridLayout *DuUI::createGridLayout(bool isSubLayout, QWidget *parent)
{
    QGridLayout *layout = new QGridLayout(parent);
    setupLayout(layout, isSubLayout);
    return layout;
}

QGridLayout *DuUI::addGridLayout(QWidget *parent)
{
    return createGridLayout(false, parent);
}

QGridLayout *DuUI::addGridLayout(QBoxLayout *parent)
{
    QGridLayout *layout = createGridLayout(true);
    parent->addLayout(layout);
    return layout;
}

QStackedLayout *DuUI::createStackedLayout(bool isSubLayout, QWidget *parent)
{
    auto *layout = new QStackedLayout(parent);
    setupLayout(layout, isSubLayout);
    return layout;
}

QStackedLayout *DuUI::addStackedLayout(QWidget *parent)
{
    return createStackedLayout(false, parent);
}

QStackedLayout *DuUI::addStackedLayout(QBoxLayout *parent)
{
    QStackedLayout *layout = createStackedLayout(true);
    parent->addLayout(layout);
    return layout;
}

void DuUI::centerLayout(QLayout *child, QWidget *parent, int vstretch, int hstretch)
{
    auto hLayout = DuUI::addBoxLayout(Qt::Horizontal, parent);
    hLayout->addStretch(100);
    auto vLayout = DuUI::addBoxLayout(Qt::Vertical, hLayout);
    vLayout->addStretch(100);
    vLayout->addLayout(child);
    vLayout->addStretch(100);
    hLayout->addStretch(100);

    vLayout->setStretch(1, vstretch);
    hLayout->setStretch(1, hstretch);
}

QWidget *DuUI::addBlock(QLayout *child, QBoxLayout *parent)
{
    auto w = new QWidget();
    w->setProperty("class", "block");
    w->setLayout(child);
    parent->addWidget(w);

    setupLayout(child);

    return w;
}
