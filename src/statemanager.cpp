#include "statemanager.h"
#include "daemon.h"
#include "ramserverinterface.h"
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

void StateManager::logout(QString reopenFile)
{
    // Release
    m_app->detach();

    // Stop the daemon
    Daemon::instance()->stop();
    RamServerInterface::instance()->eraseUserPassword();
    DBInterface::instance()->setOffline();

    m_app->restart(QStringList(reopenFile));
}

void StateManager::quit(bool sync)
{
    // Release
    m_app->detach();

    // Stop the daemon
    Daemon::instance()->stop();
    DBInterface::instance()->setOffline(sync);

    m_app->quit();
}

void StateManager::restart(bool sync)
{
    // Release
    m_app->detach();

    // Stop the daemon
    Daemon::instance()->stop();
    DBInterface::instance()->setOffline(sync);

    m_app->restart();
}

void StateManager::open(QString filePath)
{
    qDebug() << "Opening file: " << filePath;

    QString currentFile = DBInterface::instance()->dataFile();
    if (currentFile == "") {
        // Set database
        DBInterface::instance()->setDataFile( filePath );
        // Trigger a full sync
        if (RamServerInterface::instance()->isOnline()) DBInterface::instance()->fullSync();
        return;
    }

    logout(filePath);
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
