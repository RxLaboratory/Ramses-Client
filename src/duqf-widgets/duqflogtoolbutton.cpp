#include "duqflogtoolbutton.h"
#include "duqf-widgets/duicon.h"
#include "duqf-widgets/duqfloggingtextedit.h"

DuQFLogToolButton::DuQFLogToolButton(QWidget *parent): QToolButton(parent)
{
    setupUi();

    clear();

    connect(DuQFLogger::instance(), &DuQFLogger::newLog, this, &DuQFLogToolButton::log);
    connect(this, SIGNAL(clicked()), this, SLOT(showLog()));
}

void DuQFLogToolButton::log(DuQFLog m)
{
    DuQFLog::LogType t = m.type();

    if (t > _currentLevel)
    {
        _currentLevel = t;

        switch(t)
        {
        case DuQFLog::Data: break;
        case DuQFLog::Debug: break;
        case DuQFLog::Information: break;
        case DuQFLog::Warning:
            this->show();
            this->setIcon(DuIcon(":/icons/warning"));
            this->setToolTip("Warning! Some errors have occured.");
            break;
        case DuQFLog::Critical:
            this->show();
            this->setIcon(DuIcon(":/icons/critical"));
            this->setToolTip("There are critical errors. You should post a bug report and restart the application. Be careful, you may lose some data.");
            break;
        case DuQFLog::Fatal:
            this->show();
            this->setIcon(DuIcon(":/icons/fatal"));
            this->setToolTip("A fatal error has occured, we can't continue. Please file a bug report.");
            break;
        }
    }

    if (m_autoShowLog) showLog();
}

void DuQFLogToolButton::showLog()
{
    if (_currentLevel <= DuQFLog::Information) return;
    _logDialog->show();
}

void DuQFLogToolButton::setupUi()
{
    this->setText("");
    this->setToolButtonStyle(Qt::ToolButtonIconOnly);

    _logDialog = new QDialog(this);
    _logDialog->setModal(true);
    _logDialog->setMinimumHeight(320);
    _logDialog->setMinimumWidth(360);
    QVBoxLayout *layout = new QVBoxLayout(_logDialog);
    layout->setContentsMargins(3,3,3,3);
    layout->setSpacing(3);

    DuQFLoggingTextEdit *logWidget = new DuQFLoggingTextEdit();
    logWidget->setLevel(DuQFLog::Warning);
    layout->addWidget(logWidget);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setContentsMargins(0,0,0,0);
    buttonLayout->setSpacing(3);
    layout->addLayout(buttonLayout);

    QPushButton *cButton = new QPushButton();
    cButton->setText("Clear");
    cButton->setIcon(DuIcon(":/icons/clean"));
    buttonLayout->addWidget(cButton);

    QPushButton *closeButton = new QPushButton();
    closeButton->setText("Close");
    closeButton->setIcon(DuIcon(":/icons/close"));
    buttonLayout->addWidget(closeButton);

    _logDialog->setLayout(layout);

    _logDialog->setStyleSheet("QDialog, QTextEdit { background-color:#333; }");

    connect(cButton, SIGNAL(clicked()), logWidget, SLOT(clear()));
    connect(cButton, SIGNAL(clicked()), this, SLOT(clear()));
    connect(closeButton, SIGNAL(clicked()), _logDialog, SLOT(reject()));
}

void DuQFLogToolButton::setAutoShowLog(bool newAutoShowLog)
{
    m_autoShowLog = newAutoShowLog;
}

void DuQFLogToolButton::clear()
{
    this->hide();
    _currentLevel = DuQFLog::Information;
    this->setIcon(DuIcon(":/icons/ok"));
    _logDialog->accept();
}
