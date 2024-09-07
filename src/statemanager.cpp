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
    // Release
    m_app->detach();

    // Stop the daemon
    Daemon::instance()->stop();
    DBInterface::i()->setOffline(sync);

    m_app->quit();
}

void StateManager::restart(bool sync, const QString &dbFile)
{
    // Release
    m_app->detach();

    // Stop the daemon
    Daemon::instance()->stop();
    DBInterface::i()->setOffline(sync);

    m_app->restart(QStringList(dbFile));
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
