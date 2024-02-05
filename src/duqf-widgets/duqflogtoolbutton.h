#ifndef DUQFLOGTOOLBUTTON_H
#define DUQFLOGTOOLBUTTON_H

#include <QToolButton>
#include <QDialog>
#include <QVBoxLayout>
#include <QSettings>
#include "duqf-utils/duqflogger.h"

class DuQFLogToolButton : public QToolButton
{
    Q_OBJECT
public:
    DuQFLogToolButton(QWidget *parent = nullptr);
    void setAutoShowLog(bool newAutoShowLog);
signals:
    void visibilityChanged(bool);
private slots:
    void log(DuQFLog m);
    void showLog();
    void clear();
private:
    void setupUi();
    DuQFLog::LogType _currentLevel;
    QDialog *_logDialog;
    bool m_autoShowLog = true;
};

#endif // DUQFLOGTOOLBUTTON_H
