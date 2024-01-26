#ifndef DULOGGER_H
#define DULOGGER_H

#include <QObject>
#include <QHash>
#include <QRegularExpression>

#include "enums.h"

class DuLogger : public QObject
{
    Q_OBJECT
public:
    static DuLogger *i();
    void setAsMessageHandler(LogType loglevel = InformationLog);
    void registerComponent(const QString &name, QObject *component);

    static void messageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg);

public slots:
    void log(const QString &log, LogType type = InformationLog, const QString &component = "");

signals:
    void newLog(QString message, LogType type, QString);

private:
    explicit DuLogger(QObject *parent = nullptr): QObject(parent) {};
    void printErr(const QString &msg);
    void printOut(const QString &msg);

    static DuLogger *_instance;
    static const QRegularExpression _componentRE;
    QHash<QObject*,QString> _components;
    LogType _cliLevel = InformationLog;
};

#endif // DULOGGER_H
