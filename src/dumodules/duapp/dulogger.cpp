#include "dulogger.h"

#include <QtDebug>

const QRegularExpression DuLogger::_componentRE = QRegularExpression("{(\\w+)}");

DuLogger *DuLogger::_instance = nullptr;


DuLogger *DuLogger::i()
{
    if (!_instance) _instance = new DuLogger();
    return _instance;
}

void DuLogger::setAsMessageHandler(LogType loglevel)
{
#ifdef QT_DEBUG
    Q_UNUSED(loglevel)
    _cliLevel = DataLog;
#else
    _cliLevel = loglevel;
#endif
    // handles messages from the app and redirects them to stdout (info) or stderr (debug, warning, critical, fatal)
    qSetMessagePattern("%{type} : %{if-debug}%{function} | %{endif}%%{if-warning}%{function} | %{endif}%{message}");
    qInstallMessageHandler(DuLogger::messageOutput);
    qDebug() << "Logger ready.";
}

void DuLogger::registerComponent(const QString &name, QObject *component)
{
    _components.insert(component, name);
}

void DuLogger::messageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QString component = "";
    QString m = msg;
    QRegularExpressionMatch match = _componentRE.match(msg);
    if (match.hasMatch()) component = match.captured(1);
    m.replace(_componentRE, "");

    auto logger = i();
#ifdef QT_DEBUG
    QString dstr = QStringLiteral("%1 (%2) %3 | %4");
    switch (type) {
    case QtDebugMsg:
        logger->log(dstr.arg(context.file).arg(context.line).arg(context.function).arg(m), DebugLog, component);
        break;
    case QtInfoMsg:
        logger->log(m, InformationLog, component);
        break;
    case QtWarningMsg:
        logger->log(dstr.arg(context.file).arg(context.line).arg(context.function).arg(m), WarningLog, component);
        break;
    case QtCriticalMsg:
        logger->log(dstr.arg(context.file).arg(context.line).arg(context.function).arg(m), CriticalLog, component);
        break;
    case QtFatalMsg:
        logger->log(dstr.arg(context.file).arg(context.line).arg(context.function).arg(m), FatalLog, component);
        break;
    }
#else
    switch (type) {
    case QtDebugMsg:
        logger->log(m, DebugLog, component);
        break;
    case QtInfoMsg:
        logger->log(m, InformationLog, component);
        break;
    case QtWarningMsg:
        // Warnings are really meant for debug (at least in release builds)
        logger->log(m, DebugLog, component);
        break;
    case QtCriticalMsg:
        logger->log(m, CriticalLog, component);
        break;
    case QtFatalMsg:
        logger->log(m, FatalLog, component);
        break;
    }
#endif
}

void DuLogger::log(const QString &log, LogType type, const QString &component)
{
    QString c;
    QString componentName = component;
    if (componentName == "") componentName = _components.value( sender(), component );
    if (componentName != "") c = componentName + " | ";

    switch (type) {
    case DataLog:
#ifdef QT_DEBUG
        printErr(c + "Data: " + log);
#endif
        break;
    case DebugLog:
#ifdef QT_DEBUG
        printErr(c + "Debug: " + log);
#else
        if (_cliLevel <= DebugLog)
            printErr(c + "Debug: " + log);
#endif
        break;
    case InformationLog:
        if (_cliLevel <= InformationLog)
            printOut(c + log);
        break;
    case WarningLog:
        if (_cliLevel <= WarningLog)
            printErr(c + "Warning: " + log);
        break;
    case CriticalLog:
        if (_cliLevel <= CriticalLog)
            printErr(c + "Critical: " + log);
        break;
    case FatalLog:
        if (_cliLevel <= FatalLog)
            printErr(c + "Fatal: " + log);
        break;
    }

    emit newLog(log, type, componentName);
}

void DuLogger::printErr(const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    fprintf(stderr, "%s\n", localMsg.constData() );
}

void DuLogger::printOut(const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    fprintf(stdout, "%s\n", localMsg.constData() );
}
