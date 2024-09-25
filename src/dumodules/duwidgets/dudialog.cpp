#include "dudialog.h"

#include <QMouseEvent>
#include <QPainter>
#include <QStyleOption>

#include "duapp/app-version.h"
#include "duapp/dusettings.h"
#include "duapp/duui.h"
#include "duwidgets/duicon.h"
#include "duwidgets/dutoolbarspacer.h"

DuDialog::DuDialog(QWidget *parent, Qt::WindowFlags f):
    QDialog(parent,f)
{
    this->setWindowFlag(Qt::FramelessWindowHint, true);
    this->setAttribute(Qt::WA_TranslucentBackground, true);
    this->setSizeGripEnabled(true);
    this->setWindowIcon(QIcon(APP_ICON));

    _margin = DuSettings::i()->get(DuSettings::UI_Margins).toInt();

    // A layout
    ui_layout = DuUI::addBoxLayout(Qt::Vertical, this);
    // More space
    ui_layout->setSpacing(_margin*2);
    // Room for the size grip
    QMargins ms = ui_layout->contentsMargins();
    ms.setBottom(12);
    ui_layout->setContentsMargins(ms);

    // Add a title bar
    ui_titleBar = new QToolBar(this);
    ui_titleBar->setIconSize(QSize(12,12));
    ui_titleBar->setContextMenuPolicy(Qt::PreventContextMenu);
    ui_layout->addWidget(ui_titleBar);

    // Title
    ui_titleLabel = new QLabel(STR_FILEDESCRIPTION, this);
    ui_titleBar->addWidget(ui_titleLabel);

    // Spacer
    auto spacer = new DuToolBarSpacer(this);
    ui_titleBar->addWidget(spacer);

    // The close button
    ui_closeButton  = new QToolButton(this);
    ui_closeButton->setIcon(DuIcon(":/icons/close"));
    ui_closeButton->setToolTip(tr("Cancel"));
    ui_closeButton->setIconSize(QSize(12,12));
    ui_titleBar->addWidget(ui_closeButton);
    connect(ui_closeButton, &QToolButton::clicked,
            this, &QDialog::close);

    ui_titleBar->installEventFilter(this);
}

void DuDialog::setWindowTitle(const QString &title)
{
    QDialog::setWindowTitle(title);
    ui_titleLabel->setText(title);
}

int DuDialog::exec()
{
    if (_launching) {
        int r = QDialog::exec();
        _launching = false;
        return r;
    }

    _launching = true;
    return DuUI::execDialog(this);
}

bool DuDialog::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent *mouseEvent = (QMouseEvent*)event;
        if (mouseEvent->button() == Qt::LeftButton)
        {
            _toolbarClicked = true;
            _dragPos = mouseEvent->globalPos() - this->frameGeometry().topLeft();
            event->accept();
        }
        return true;
    }
    else if (event->type() == QEvent::MouseMove)
    {
        QMouseEvent *mouseEvent = (QMouseEvent*)event;
        if (mouseEvent->buttons() & Qt::LeftButton && _toolbarClicked)
        {
            this->move(mouseEvent->globalPos() - _dragPos);
            event->accept();
        }
        return true;
    }
    else if (event->type() == QEvent::MouseButtonRelease)
    {
        _toolbarClicked = false;
        return true;
    }

    // standard event processing
    return QObject::eventFilter(obj, event);

}

void DuDialog::paintEvent(QPaintEvent *ev)
{
    // Initialize the painter.
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    // Have style sheet?
    if (testAttribute(Qt::WA_StyleSheetTarget)) {
        // Let QStylesheetStyle have its way with us.
        QStyleOption opt;
        opt.initFrom(this);
        style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
        p.end();
        return;
    }

    // Paint thyself.
    QRectF rect = ev->rect();
    // Check for a border size.
    // Look up the default border (frame) width for the current style.
    QStyleOption opt;
    opt.initFrom(this);
    int penWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth, &opt, this);
    // Got pen?
    if (penWidth > 0.0) {
        p.setPen(QPen(palette().brush(foregroundRole()), penWidth));
        // Ensure border fits inside the available space.
        const qreal dlta = penWidth * 0.5;
        rect.adjust(dlta, dlta, -dlta, -dlta);
    }
    else {
        // QPainter comes with a default 1px pen when initialized on a QWidget.
        p.setPen(Qt::NoPen);
    }
    // Set the brush from palette role.
    p.setBrush(palette().brush(backgroundRole()));
    // Radius
    p.drawRoundedRect(rect, _margin*2, _margin*2, Qt::AbsoluteSize);

    // C'est finí
    p.end();
}
