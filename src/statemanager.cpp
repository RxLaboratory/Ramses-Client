#include "statemanager.h"
#include "daemon.h"
#include "dbinterface.h"
#include "duapp/dulogger.h"

StateManager *StateManager::_instance = nullptr;

StateManager *StateManager::i()
{
    if (!_instance) _instance = new StateManager();
    return _instance;
}

StateManager::State StateManager::state() const
{
    if (m_tempState != Unknown)
        return m_tempState;
    return m_state;
}

bool StateManager::isDBBusy() const
{
    return (m_state == LoadingDataBase ||
            m_state == WritingDataBase );
}

void StateManager::quit(bool sync)
{
    if (m_state==Closing)
        return;

    setTitle(tr("Closing..."));
    qInfo().noquote() << "Unlocking unique instance...";

    setState(Closing);

    // Release
    m_app->detach();

    qInfo().noquote() << "Stopping the Ramses Daemon...";

    // Stop the daemon
    Daemon::instance()->stop();

    // One last sync

    qInfo().noquote() << "One last sync...";

    if (sync) {
        connect(DBInterface::i(), &DBInterface::syncFinished,
                m_app, &DuApplication::quit);

        if (DBInterface::i()->fullSync())
            return;
    }

    qInfo().noquote() << "Bye!";

    m_app->quit();
}

void StateManager::restart(bool sync, const QString &dbFile)
{
    if (m_state==Closing)
        return;

    setState(Closing);

    // Release
    m_app->detach();

    // Stop the daemon
    Daemon::instance()->stop();

    // One last sync
    if (sync) {
        connect(DBInterface::i(), &DBInterface::syncFinished,
                this, [this, dbFile] () {
                    m_app->restart(QStringList(dbFile));
                });

        if (DBInterface::i()->fullSync())
            return;
    }

    m_app->restart(QStringList(dbFile));
}

void StateManager::forceQuit()
{
    setState(Closing);
    m_app->detach();
    Daemon::instance()->stop();
    m_app->quit();
}

void StateManager::setState(State newState)
{
    if (newState == m_state) return;

    m_previousState = m_state;
    m_state = newState;
    m_tempState = Unknown;

    qDebug().noquote() << "App state changed to" << m_state;
    emit stateChanged(m_state);
}

void StateManager::setTempState(State tempState)
{
    m_tempState = tempState;

    if (m_tempState != Unknown) {
        qDebug().noquote() << "App state temporarily changed to" << m_tempState;
        emit stateChanged(m_tempState);
    }
    else
        emit stateChanged(m_state);
}

void StateManager::keepTempState()
{
    if (m_tempState == Unknown)
        return;
    m_state = m_tempState;
    m_tempState = Unknown;
}

StateManager::StateManager(QObject *parent)
    : QObject{parent}
{
    m_app = qobject_cast<DuApplication*>(qApp);

    connect(DuLogger::i(), &DuLogger::newLog,
            this, [this] (const QString &m, LogType t) {
        if (t > LogType::DebugLog) setText(m);
    });
}

void StateManager::setText(const QString &newText)
{
    if (m_text == newText)
        return;
    m_text = newText;
    emit textChanged(m_text);
}

void StateManager::setTitle(const QString &newTitle)
{
    if (m_title == newTitle)
        return;
    m_title = newTitle;
    emit titleChanged(m_title);
}
