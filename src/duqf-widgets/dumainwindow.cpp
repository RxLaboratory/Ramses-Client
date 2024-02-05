#include "dumainwindow.h"

#include <QtDebug>
#include <QToolBar>
#include <QMouseEvent>
#include <QVariantAnimation>
#include <QPointer>

#include "duqf-app/duui.h"
#include "duqf-app/dusettingsmanager.h"
#include "duqf-widgets/dutoolbarspacer.h"
#include "qtabbar.h"
#include "duqf-app/app-version.h"

DuMainWindow::DuMainWindow(QWidget *parent)
    : QMainWindow{parent}
{
    qDebug() << "Create Main Window";
    // Build the actions
    setupActions();
    // Build the window
    setupUi();
    // Build the toolbar
    setupToolBar();
    // Events
    connectEvents();
}

void DuMainWindow::setWindowTitle(const QString &title)
{
    if (title != "") QMainWindow::setWindowTitle(QStringLiteral(STR_INTERNALNAME) + " - " + title);
    else QMainWindow::setWindowTitle(QStringLiteral(STR_INTERNALNAME));
    ui_titleLabel->setText(title);
}

void DuMainWindow::setMaximizedState(bool maximized)
{
    m_maximizeAction->setChecked(maximized);
    m_maximized = maximized;
}

void DuMainWindow::addDockWidget(Qt::DockWidgetArea area, DuDockWidget *dockwidget)
{
    QMainWindow::addDockWidget(area, dockwidget);
    dockwidget->installEventFilter(this);
    ui_docks << dockwidget;
}

void DuMainWindow::setupUi()
{
    this->setWindowIcon(QIcon(":/icons/app"));
    QMainWindow::setWindowTitle(QStringLiteral(STR_INTERNALNAME));

    QWidget *centralWidget = new QWidget(this);
    this->setCentralWidget(centralWidget);

    ui_mainLayout = new QVBoxLayout(centralWidget);
}

void DuMainWindow::setupToolBar()
{
    ui_mainToolBar = new QToolBar(this);
    ui_mainToolBar->setMovable(false);
    this->addToolBar(Qt::TopToolBarArea, ui_mainToolBar);

// === Adjust toolbar Settings ===

#ifdef Q_OS_WIN
    ui_mainToolBar->setProperty("osClass", "win");
#elif defined(Q_OS_MAC)
    ui_mainToolBar->setProperty("osClass", "mac");
    ui_mainToolBar->installEventFilter(this);
#elif defined(Q_OS_LINUX)
    ui_mainToolBar->setProperty("osClass", "linux");
    ui_mainToolBar->installEventFilter(this);
#endif

    // Appearance
    ui_mainToolBar->setObjectName("mainToolBar");
    ui_mainToolBar->setIconSize(
        DuUI::adjustToDpi(QSize(16,16))
        );
    ui_mainToolBar->setWindowTitle(QString(STR_FILEDESCRIPTION) + " main toolbar");
    ui_mainToolBar->setStyleSheet( DuUI::css("mainToolBar") );

    // remove right click on toolbar
    ui_mainToolBar->setContextMenuPolicy(Qt::PreventContextMenu);
}

void DuMainWindow::connectEvents()
{
    // Window Buttons
#ifdef Q_OS_WIN
    connect(m_minimizeAction, &QAction::triggered, this, &DuMainWindow::minimizeTriggered);
    connect(m_maximizeAction, &QAction::triggered, this, &DuMainWindow::maximizeTriggered);
    connect(m_closeAction, &QAction::triggered, this, &DuMainWindow::close);
#endif
}

void DuMainWindow::setDockVisible(DuDockWidget *w, bool visible)
{
    // If it's already visible, call show
    if (w->isVisible() == visible) {
        w->setVisible(visible);
        m_dockInteraction = nullptr;
        return;
    }
    // If the dock is floating, just show
    if (w->isFloating()) {
        w->setVisible(visible);
        m_dockInteraction = nullptr;
        return;
    }
    // Same if it's tabbed with other (visible and docked) widgets
    const auto tabs = tabifiedDockWidgets(w);
    for(auto tab: tabs) {
        if (tab->isVisible() == visible && !tab->isFloating()) {
            w->setVisible(visible);
            m_dockInteraction = nullptr;
            return;
        }
    }

    QVariantAnimation *anim = new QVariantAnimation(this);
    anim->setDuration(200);
    if (visible) anim->setStartValue(1);
    else anim->setEndValue(1);
    anim->setEasingCurve(
        QEasingCurve( QEasingCurve::InOutCubic )
        );

    // We need to be able to scale this down
    QPointer<QWidget> wi = w->widget();
    QSizePolicy cP;
    QSize cS;
    if (wi) {
        cP = wi->sizePolicy();
        cS = wi->minimumSize();
    }
    QPointer<QWidget> tb = w->titleBarWidget();
    QSizePolicy cTP;
    QSize cTS;
    if (tb) {
        cTP = w->titleBarWidget()->sizePolicy();
        cTS = w->titleBarWidget()->minimumSize();
    }

    Qt::Orientation o;

    // Get/Set the size
    QSize s;
    if (visible) {
        s = DuSettingsManager::instance()->uiDockSize( w->objectName() );
        if (s.isEmpty()) s = w->sizeHint();
    }
    else {
        s = w->size();
        DuSettingsManager::instance()->setUIDockSize( w->objectName(),s );
    }

    Qt::DockWidgetArea a = dockWidgetArea(w);
    if (a == Qt::TopDockWidgetArea || a == Qt::BottomDockWidgetArea) {
        if (visible) anim->setEndValue(s.height());
        else anim->setStartValue(s.height());
        o = Qt::Vertical;
        if (wi) {
            wi->setSizePolicy(cP.horizontalPolicy(), QSizePolicy::Ignored);
            wi->setMinimumSize(cS.width(),1); // 0 is ignored, the smallest size is 1px
        }
        if (tb) {
            tb->setSizePolicy(cTP.horizontalPolicy(), QSizePolicy::Ignored);
            tb->setMinimumSize(cTS.width(),1);
        }
    }
    else {
        if (visible) anim->setEndValue(s.width());
        else anim->setStartValue(s.width());
        o = Qt::Horizontal;
        if (wi) {
            wi->setSizePolicy(QSizePolicy::Ignored, cP.verticalPolicy());
            wi->setMinimumSize(1,cS.height()); // 0 is ignored, the smallest size is 1px
        }
        if (tb) {
            tb->setSizePolicy(QSizePolicy::Ignored, cTP.verticalPolicy());
            tb->setMinimumSize(1,cTS.height());
        }
    }

    // Animate
    connect(anim, &QVariantAnimation::valueChanged,
            this, [this, w, o](QVariant v){
                resizeDocks({w}, {v.toInt()}, o);
            });

    // Restore current values
    connect(anim, &QVariantAnimation::finished,
            this, [this, anim, w, wi, tb, cP, cS, cTP, cTS, visible] {
                if (wi) {
                    wi->setSizePolicy(cP);
                    wi->setMinimumSize(cS);
                }
                if (tb) {
                    tb->setSizePolicy(cTP);
                    tb->setMinimumSize(cTS);
                }
                if (!visible) w->hide();
                anim->deleteLater();
                m_dockInteraction = nullptr;
            });

    if (visible) resizeDocks({w}, {1}, o);
    anim->start(QVariantAnimation::DeleteWhenStopped);

    if (visible) w->show();
    setDockIcons();
}

void DuMainWindow::setDockIcons()
{
    const auto tabBars = findChildren<QTabBar*>();
    for(auto tabBar: tabBars) {
        for (int i = 0; i < tabBar->count(); i++) {
            quintptr d = tabBar->tabData(i).toULongLong();
            for(auto dock: qAsConst(ui_docks)) {
                if (d == reinterpret_cast<quintptr>(dock)) {
                    QIcon icon = dock->windowIcon();
                    if (!icon.isNull()) {
                        tabBar->setTabIcon(i, dock->windowIcon());
                        tabBar->setTabText(i, "");
                    }
                    break;
                }
            }
        }
    }
}

bool DuMainWindow::eventFilter(QObject *obj, QEvent *event)
{
    // Check the object
    bool onToolbar = obj == ui_mainToolBar;
    bool onDock = false;
    auto dock = qobject_cast<DuDockWidget*>(obj);
    if (dock) onDock = true;

    // Check type
    auto type = event->type();

    if (type == QEvent::MouseButtonPress)
    {
        if (onDock) m_dockInteraction = dock;
        if (onToolbar) {
            QMouseEvent *mouseEvent = (QMouseEvent*)event;
            if (mouseEvent->button() == Qt::LeftButton)
            {
                m_toolBarClicked = true;
                m_dragPosition = mouseEvent->globalPos() - this->frameGeometry().topLeft();
                event->accept();
            }
            return true;
        }
    }
    else if (type == QEvent::MouseMove)
    {
        if (onToolbar) {
            if (this->isMaximized()) return false;
            QMouseEvent *mouseEvent = (QMouseEvent*)event;
            if (mouseEvent->buttons() & Qt::LeftButton && m_toolBarClicked)
            {
                this->move(mouseEvent->globalPos() - m_dragPosition);
                event->accept();
            }
            return true;
        }
    }
    else if (type == QEvent::MouseButtonRelease)
    {
        if (onDock) m_dockInteraction = nullptr;
        if (onToolbar) {
            m_toolBarClicked = false;
            return true;
        }
    }
#ifndef Q_OS_MAC
    else if (type == QEvent::MouseButtonDblClick)
    {
        if (onToolbar) {
            if (this->isMaximized()) this->showNormal();
            else this->showMaximized();
            event->accept();
            return true;
        }
    }
#endif
    else if (type == QEvent::Show || type == QEvent::Hide) {
        if (onDock && !m_dockInteraction) {
            m_dockInteraction = dock;
            // Revert
            bool v = type == QEvent::Show;
            dock->setVisible(!v);
            // Animate
            setDockVisible(dock, v);
            event->accept();
            return true;
        }
    }

    // standard event processing
    return QObject::eventFilter(obj, event);
}

void DuMainWindow::addWindowButtons(bool showVersion)
{
    ui_mainToolBar->addWidget(new DuToolBarSpacer());

    QString labelTxt = QStringLiteral(STR_FILEDESCRIPTION);
    if (showVersion) labelTxt += " " + QStringLiteral(STR_VERSION);

    ui_titleLabel = new QLabel(labelTxt);
    ui_titleLabel->setAttribute(Qt::WA_TransparentForMouseEvents);
    ui_mainToolBar->addWidget(ui_titleLabel);

    ui_mainToolBar->addWidget(new DuToolBarSpacer());

#ifdef Q_OS_WIN
    ui_mainToolBar->addAction(m_minimizeAction);
    QWidget *minWidget = ui_mainToolBar->widgetForAction(m_minimizeAction);
    minWidget->setFixedSize(
        DuUI::adjustToDpi(QSize(24,24))
        );
    minWidget->setObjectName("windowButton");
    ui_mainToolBar->layout()->setAlignment(minWidget, Qt::AlignTop);

    ui_mainToolBar->addAction(m_maximizeAction);
    QWidget *maxWidget = ui_mainToolBar->widgetForAction(m_maximizeAction);
    maxWidget->setFixedSize(
        DuUI::adjustToDpi(QSize(24,24))
        );
    maxWidget->setObjectName("windowButton");
    ui_mainToolBar->layout()->setAlignment(maxWidget, Qt::AlignTop);

    ui_mainToolBar->addAction(m_closeAction);
    QWidget *closeWidget = ui_mainToolBar->widgetForAction(m_closeAction);
    closeWidget->setFixedSize(
        DuUI::adjustToDpi(QSize(24,24))
        );
    closeWidget->setObjectName("windowButton");
    ui_mainToolBar->layout()->setAlignment(closeWidget, Qt::AlignTop);

#endif // WIN
}

void DuMainWindow::setStyle()
{
    // Font first so it's available in the css
    DuUI::setFont("Ubuntu");
    DuUI::setAppCss( DuUI::css() );
    DuUI::setAppToolButtonStyle(
        DuSettingsManager::instance()->uiToolButtonStyle()
        );
}

void DuMainWindow::setupActions()
{
#ifdef Q_OS_WIN
    m_minimizeAction = new DuAction(this);
    m_minimizeAction->setIcon(DuIcon(":/icons/minimize"));

    m_maximizeAction = new DuAction(this);
    auto maximizeIcon = new DuSVGIconEngine(":/icons/move-up");
    maximizeIcon->setCheckedFile(":/icons/move-down");
    maximizeIcon->setCheckedColor( maximizeIcon->mainColor() );
    m_maximizeAction->setIcon(maximizeIcon);
    m_maximizeAction->setCheckable(true);

    m_closeAction = new DuAction(this);
    m_closeAction->setIcon(DuIcon(":/icons/close"));
#endif // WIN
}
