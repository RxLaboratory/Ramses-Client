#include "dumainwindow.h"

#include <QtDebug>
#include <QToolBar>
#include <QMouseEvent>
#include <QVariantAnimation>
#include <QPointer>
#include <QTabBar>

#include "duapp/duui.h"
#include "duapp/dusettings.h"
#include "duapp/app-version.h"

DuMainWindow::DuMainWindow(QWidget *parent)
    : QGoodWindow{parent}
{
    qDebug() << "Create Main Window";
    // Build the actions
    setupActions();
    // Build the window
    setupUi();
    // Build the toolbar
    setupTitleBar();
}

void DuMainWindow::setWindowTitle(const QString &title)
{
    QString t = QStringLiteral(STR_FILEDESCRIPTION);
    if (title != "")
        t += " - " + title;
    QGoodWindow::setWindowTitle(t);
    ui_titleLabel->setText(t);
}

void DuMainWindow::addDockWidget(Qt::DockWidgetArea area, QDockWidget *dockwidget)
{
    ui_centralMainWidget->addDockWidget(area, dockwidget);
    dockwidget->installEventFilter(m_dockEventFilter);
    connect(dockwidget, &QDockWidget::dockLocationChanged,
            this, &DuMainWindow::setDockIcons);
    ui_docks << dockwidget;
}

const QVector<QDockWidget *> &DuMainWindow::docks() const
{
    return ui_docks;
}

int DuMainWindow::execDialog(QDialog *dialog) const
{
    return QGoodCentralWidget::execDialogWithWindow(dialog, const_cast<DuMainWindow*>(this), ui_goodCentralWidget);
}

void DuMainWindow::closeEvent(QCloseEvent *event)
{
    DuSettings::i()->set(
        DuSettings::UI_WindowGeometry,
        this->saveGeometry()
        );
    DuSettings::i()->set(
        DuSettings::UI_WindowState,
        this->saveState()
        );
    QGoodWindow::closeEvent(event);
}

void DuMainWindow::hideTitle()
{
    ui_titleLabel->setVisible(false);
}

void DuMainWindow::hideAllDocks()
{
    for(auto dock: qAsConst(ui_docks))
        dock->hide();
}

void DuMainWindow::updateWindow()
{
    ui_goodCentralWidget->updateWindow();
}

void DuMainWindow::setupUi()
{
    ui_goodCentralWidget = new QGoodCentralWidget(this);
    // Setup the good central widget
    ui_goodCentralWidget->setActiveBorderColor(
        DuUI::pushColor(
            DuSettings::i()->get(DuSettings::UI_BackgroundColor).value<QColor>(), 1.5
            ));
    ui_goodCentralWidget->setTitleVisible(false);
    ui_goodCentralWidget->setIconVisibility(QGoodCentralWidget::IconVisibilityType::IconHiddenAcceptMouseInput);

    // Contents of the window
    // Needs a child mainwindow for the docks
    ui_centralMainWidget = new QMainWindow(this);
    ui_centralWidget = new QWidget(ui_centralMainWidget);
    // set as central widget
    ui_centralMainWidget->setCentralWidget(ui_centralWidget);
    // Add it to the good central widget
    ui_goodCentralWidget->setCentralWidget(ui_centralMainWidget);

    //Set the central widget of QGoodWindow which is QGoodCentralWidget
    this->setCentralWidget(ui_goodCentralWidget);

    // Dock event filter
    m_dockEventFilter = new DuDockEventFilter(this);
    connect(m_dockEventFilter, &DuDockEventFilter::dockVisibilityChanged, this, &DuMainWindow::setDockVisible);
}

void DuMainWindow::setupTitleBar()
{
    // LEFT
    ui_leftToolBar = new QToolBar(this);
    ui_leftToolBar->setMovable(false);
    ui_leftToolBar->setObjectName("leftToolBar");
    ui_leftToolBar->setIconSize( QSize(16,16) );
    ui_leftToolBar->setMinimumHeight(16);
    ui_leftToolBar->setWindowTitle(QString(STR_FILEDESCRIPTION) + " | " + tr("left toolbar"));
    ui_leftToolBar->setContextMenuPolicy(Qt::PreventContextMenu);
    ui_goodCentralWidget->setLeftTitleBarWidget(ui_leftToolBar, true);

    // Middle
    ui_centerToolBar = new QToolBar(this);
    ui_centerToolBar->setMovable(false);
    ui_centerToolBar->setObjectName("centerToolBar");
    ui_centerToolBar->setIconSize( QSize(16,16) );
    ui_centerToolBar->setMinimumHeight(16);
    ui_centerToolBar->setWindowTitle(QString(STR_FILEDESCRIPTION) + " | " + tr("center toolbar"));
    ui_centerToolBar->setContextMenuPolicy(Qt::PreventContextMenu);
    ui_goodCentralWidget->setCenterTitleBarWidget(ui_centerToolBar, true);

    // RIGHT
    ui_rightToolBar = new QToolBar(this);
    ui_rightToolBar->setMovable(false);
    ui_rightToolBar->setObjectName("rightToolBar");
    ui_rightToolBar->setIconSize( QSize(16,16) );
    ui_rightToolBar->setMinimumHeight(16);
    ui_rightToolBar->setWindowTitle(QString(STR_FILEDESCRIPTION) + " | " + tr("right toolbar"));
    ui_rightToolBar->setContextMenuPolicy(Qt::PreventContextMenu);
    ui_goodCentralWidget->setRightTitleBarWidget(ui_rightToolBar, true);

    // A Label for the title
    ui_titleLabel = new QLabel(STR_FILEDESCRIPTION);
    ui_titleLabel->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    ui_rightToolBar->addWidget(ui_titleLabel);

    this->setWindowIcon(QIcon(":/icons/app"));
    setWindowTitle("");
}

void DuMainWindow::setDockVisible(QDockWidget *w, bool visible)
{
    // If it's already visible, call show
    if (w->isVisible() == visible) {
        w->setVisible(visible);
        return;
    }
    // If the dock is floating, just show
    if (w->isFloating()) {
        w->setVisible(visible);
        return;
    }
    // Same if it's tabbed with other (visible and docked) widgets
    const auto tabs = tabifiedDockWidgets(w);
    for(auto tab: tabs) {
        if (tab->isVisible() == visible && !tab->isFloating()) {
            w->setVisible(visible);
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
        s = DuSettings::i()->get(
                               DuSettings::UI_DockSize,
                               QVariant(),
                               w->objectName()
                               ).toSize();
        if (s.isEmpty()) s = w->sizeHint();
    }
    else {
        s = w->size();
        DuSettings::i()->set(
            DuSettings::UI_DockSize,
            s,
            w->objectName()
            );
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
                ui_centralMainWidget->resizeDocks({w}, {v.toInt()}, o);
            });

    // Restore current values
    connect(anim, &QVariantAnimation::finished,
            this, [ this, anim, w, wi, tb, cP, cS, cTP, cTS, visible] {
                if (wi) {
                    wi->setSizePolicy(cP);
                    wi->setMinimumSize(cS);
                }
                if (tb) {
                    tb->setSizePolicy(cTP);
                    tb->setMinimumSize(cTS);
                }
                if (!visible) {
                    // Actually hide
                    m_dockEventFilter->suspend(true);
                    w->hide();
                    m_dockEventFilter->suspend(false);
                }
                anim->deleteLater();
            });

    // Show right now
    if (visible) {
        ui_centralMainWidget->resizeDocks({w}, {1}, o);
        w->show();
    }

    anim->start(QVariantAnimation::DeleteWhenStopped);
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

void DuMainWindow::setStyle()
{
    // Extact fonts
    DuUI::addApplicationFonts();
    // Set CSS
    DuUI::setAppCss( DuUI::css() );
    DuUI::setAppToolButtonStyle(
        static_cast<Qt::ToolButtonStyle>(
            DuSettings::i()->get(DuSettings::UI_ToolButtonStyle).toInt()
            )
        );

    updateWindow();
}

void DuMainWindow::setupActions()
{

}

bool DuDockEventFilter::eventFilter(QObject *obj, QEvent *event)
{
    // Check the object, shouldn't be installed on something else
    auto dock = qobject_cast<QDockWidget*>(obj);
    //Q_ASSERT(dock);

    // Check type
    auto type = event->type();

    if (type == QEvent::MouseButtonPress)
    {
        m_suspended = true;
        event->ignore();
        return false;
    }
    else if (type == QEvent::MouseButtonRelease)
    {
        m_suspended = false;
        event->ignore();
        return false;
    }

    // Skip and propagate
    if (m_suspended)
        return false;

    // If we're showing or hiding
    if (type == QEvent::Show || type == QEvent::Hide) {
        // Start interaction
        m_suspended = true;

        // Revert
        bool v = type == QEvent::Show;
        dock->setVisible(!v);
        // Animate
        emit dockVisibilityChanged(dock, v);

        // Done
        event->accept();
        // Finish interaction
        m_suspended = false;
        // Stop propagation
        return true;
    }

    // propagate event
    event->ignore();
    return false;
}
