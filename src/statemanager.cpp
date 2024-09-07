#include "statemanager.h"
#include "daemon.h"
#include "dbinterface.h"

StateManager *StateManager::_instance = nullptr;

StateManager *StateManager::i()
{
    if (!_instance) _instance = new StateManager();
    return _instance;
}

StateManager::State StateManager::state() const
{
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

    setState(Closing);

    // Release
    m_app->detach();

    // Stop the daemon
    Daemon::instance()->stop();

    // One last sync
    if (sync) {
        connect(DBInterface::i(), &DBInterface::syncFinished,
                m_app, &DuApplication::quit);

        if (DBInterface::i()->fullSync())
            return;
    }

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
    m_state = newState;
    emit stateChanged(m_state);
}

StateManager::StateManager(QObject *parent)
    : QObject{parent}
{
    m_app = qobject_cast<DuApplication*>(qApp);
}
