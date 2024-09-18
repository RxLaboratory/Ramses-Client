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

void StateManager::quit(bool sync)
{
    if (m_state==Closing)
        return;

    setState(Closing, tr("Closing..."));
    qInfo().noquote() << "Unlocking unique instance...";

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

    setState(Closing, tr("Closing..."));

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
    setState(Closing, tr("Closing..."));
    m_app->detach();
    Daemon::instance()->stop();
    m_app->quit();
}

void StateManager::setState(State newState)
{
    if (_singleStateConnexion)
        disconnect(_singleStateConnexion);

    if (newState == m_state) return;

    m_previousState = m_state;
    m_state = newState;
    m_tempState = Unknown;

    emitStateChanged();
}

void StateManager::setState(State newState, const QString &title)
{
    setTitle(title);
    setState(newState);
}

void StateManager::setTempState(State tempState)
{
    m_tempState = tempState;

    emitStateChanged();
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

void StateManager::emitStateChanged()
{
    emit stateChanged(state());
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


