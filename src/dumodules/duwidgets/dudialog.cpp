#include "dudialog.h"

#include <QMouseEvent>

#include "duapp/app-version.h"
#include "duapp/dusettings.h"
#include "duapp/duui.h"
#include "duwidgets/dutoolbarspacer.h"

DuDialog::DuDialog(QWidget *parent, Qt::WindowFlags f):
    QDialog(parent,f)
{
    this->setWindowFlag(Qt::FramelessWindowHint, true);
    this->setSizeGripEnabled(true);
    this->setWindowIcon(QIcon(APP_ICON));

    // A layout
    ui_layout = DuUI::addBoxLayout(Qt::Vertical, this);
    int m = DuSettings::i()->get(DuSettings::UI_Margins).toInt();
    // More space
    ui_layout->setSpacing(m*2);
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
    ui_closeButton->setIcon(QIcon(":/icons/close"));
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
